<?
/*
* Dependent on db.inc
*/
include_once ('db.inc');
// We have two objects defined here. userDB is derived from db, but is a singleton.
// user is the standard user object.
// You also probably want to change these:
global $AGLOBAL, $config;
    $AGLOBAL['cookieuser_idnum'] = 'aLibUser';
    $AGLOBAL['cookieUserToken'] = 'aLibToken';
/**
 * An arbitrary string to hash the cookie auth token with.
 * @global string $AGLOBAL['USERCOOKIESALT']
 */
$AGLOBAL['USERCOOKIESALT'] = "alibcookiesalt";
/**
 * When the auth cookie expires (as always, in utime)
 * @global int $AGLOBAL['USERCOOKIEEXPIRE'] default is 30 days from now.
 */
$AGLOBAL['USERCOOKIEEXPIRE'] = time() +60*60*24*30; #30 days from now.

/**
 * Path for the cookie.
 * @global string $AGLOBAL['USERCOOKIEPATH']
 * @see setcookie()
 */
$AGLOBAL['USERCOOKIEPATH'] = "/"; # See the setcookie() docs for these.

/**
 * Domain for the cookie.
 * @global string $AGLOBAL['USERCOOKIEDOMAIN']
 * @see setcookie()
 */
$AGLOBAL['USERCOOKIEDOMAIN'] = NULL;
/**
 * Whether the cookie is HTTPS only or not.
 * @global string $AGLOBAL['USERCOOKIESECURE']
 * @see setcookie()
 */
$AGLOBAL['USERCOOKIESECURE'] = 0;
/**
 * Only the user object should ever call the userDB object. <i>Don't touch.</i>
 * @package aLib
 * @subpackage User
 */
class userDB extends db
  {
    private static $one_ping_only;
    
    function __construct() 
      {
        global $AGLOBAL;
        $nargs = func_num_args();
        if ($nargs > 0) 
          {
            $args = func_get_args();
            if ($nargs == 1) 
              {
                $this->setup($args[0]);
              }
            else
              {
                $this->setup($args[0], $args[1], $args[2], $args[3]);
              }
          }
        else
          {
            if (isset($AGLOBAL['USERDBURL'])) 
              {
                $this->setup($AGLOBAL['USERDBURL']);
                #	print_d('Setup with ' . $AGLOBAL['USERDBURL'],1);
                
              }
            else
              {
                $this->setup($AGLOBAL['USERDBARGS'][0], $AGLOBAL['USERDBARGS'][1], $AGLOBAL['USERDBARGS'][2], $AGLOBAL['USERDBARGS'][3]);
              }
          }
        $AGLOBAL['USERDB']&= $this;
      }
    public static 
    function singleton() 
      {
        if (!isset(self::$one_ping_only)) 
          {
            $c = __CLASS__;
            self::$one_ping_only = new $c;
          }
        return self::$one_ping_only;
      }
  }
/**
 * The all singing all dancing user object -- love it.
 * @package aLib
 * @subpackage User
 */
class user extends anObject
  {
    private $userData = array();
    private $changedData = array();
    /**
     * Whether or not to use the cache.
     * @var bool $useCache default is FALSE.
     */
    public $useCache = FALSE;
    /**
     * Directory for caching. Set BEFORE useCache.
     * @var string $cacheDir default is "alibcache/".
     */
    public $cacheDir = "alibcache/";
    /**
     * Cache expiry. Set BEFORE useCache.
     * @var bool $cacheExpire default is 3600.
     */
    public $cacheExpire = 3600;
    private $db;
    private $_isloaded;
    /**
     * Whether or not the user is logged in
     * @var bool $loggedIn
     */
    public $loggedIn;
    /**
     * Constructor.
     * @param string $uid [optional] username for the object
     */
    public 
    function __construct($uid = null) 
      {
        if (!is_null($uid)) 
          {
            $this->userData['uid'] = $uid;
          }
        global $AGLOBAL;
        $this->db = userDB::singleton();
        if (isset($AGLOBAL['USERDBCOLS'])) 
          {
            $this->_is_in_db = $AGLOBAL['USERDBCOLS'];
          }
        parent::__construct();
      }
    
    function __get($name) 
      {
        global $AGLOBAL, $debug;
        // make this populate the object if it hasn't been already
        if ((!$this->_isloaded) && isset($this->userData['uid'])) 
          {
            $debug->line("Loading User...", 9);
            $this->_loaduser();
            $debug->line("Wanted $name and got " . $this->userData[$name], 9);
          }
        return $this->userData[$name];
      }
    
    function __set($name, $value) 
      {
        global $AGLOBAL, $debug;
        // First check for special variables...
        if ($name == "useCache") 
          {
            if ($value) 
              {
                $this->db->useCache = $value;
              }
            $this->useCache = $value;
            return;
          }
        // then make this populate the object if it hasn't been already
        if ($debug->level >= 10) 
          {
            if (isset($this->$name)) 
              {
                $debug->line("Changing $name. Was:", 10);
                $debug->variable($this->$name, 10);
                $debug->line("Changing $name. To:", 10);
                $debug->variable($value);
              }
            else
              {
                $debug->line("Setting " . $name, 10);
                $debug->variable($value, 10);
              }
          }
        if (!$this->_isloaded && isset($this->userData['uid'])) 
          {
            $debug->line("Loading User...", 10);
            $this->_loaduser();
          }
        $this->userData[$name] = $value;
        $this->changedData[$name] = 1;
      }
    private 
    function _loaduser() 
      {
        global $AGLOBAL, $debug;
        if ( $this->userData['uid'] == -1 ) {
            return false;        
        }
        $q = "select * from users where username='" . $this->userData['uid'] . "' OR user_idnum='" . $this->userData['uid'] . "' OR email='" . $this->userData['uid'] . "'"; // if you change this query, change it below in commit();
        $debug->line("Loading user " . $this->userData['uid'], 9);
        $debug->variable($q, 9);
        $this->db->query($q, "GetUsers");
        if ($this->db->Size("GetUsers")) 
          {
            $this->userData = $this->db->next_record("GetUsers");
            $this->_isloaded = TRUE;
            $debug->line("Loaded.", 9);
          }
        $debug->variable($this->db, 10);
      }
    /**
     * Commits changes made to the user object to the database.
     * This is very dangerous! Any variable you set via
     * <code>$user->somevariable = "somevalue"</code> will try to
     * insert itself into the db in the column "somevariable." The
     * exception is <code>$user->extraData[]</code> which should be
     * for purely temporary things.
     */
    public 
    function commit() 
      {
        global $AGLOBAL, $debug;
        // make this make __set's go to the db.
        $temp = $this->userData;
        unset($temp['extraData']);
        $uid = $temp['uid'];
        unset($temp['uid']);
        $updateMe = array();
        foreach($temp as $key => $val) 
          {
            if ($this->changedData[$key]) 
              {
                $updateMe[$key] = $val;
              }
          }
        $sql1 = $this->db->prepare("UPDATE", "users", $updateMe, "where username='" . $temp['username'] . "'");
        $sql = preg_replace("/\`passwd\` = '([^\']+)'/", "`passwd` = PASSWORD('$1')", $sql1);
        $debug->line("Converted \"$sql1\" to \"$sql\"", 3);

        $debug->line("Committing $sql", 9);
        $retval = $this->db->query($sql);
        if ($this->useCache) 
          {
            //Remove this user from the cache. We've updated, after all.
            $q = "select * from users where username='" . $this->userData['username'] . "'";
            $this->db->cache->remove($q);
          }
        $this->updateMe = array();
        return $retval;
      }
    public 
    function addUser() 
      {
        global $AGLOBAL, $debug;
        // make this make __set's go to the db.
        $temp = $this->userData;
        unset($temp['extraData']);
        $uid = $temp['uid'];
        unset($temp['uid']);
        $sql1 = $this->db->prepare("INSERT", "users", $temp);
        $sql = preg_replace("/`passwd` = '([^\']+)'/", "`passwd` = PASSWORD('$1')", $sql1);
        $debug->line("Committing $sql", 9);
        $this->db->query($sql, 'addUserSave');
        if ($this->db->insertID('addUserSave') > 0) 
          {
            $this->user_idnum = $this->db->insertID();
            $retval = TRUE;
          }
        else
          {
            $retval = FALSE;
          }
        return $retval;
      }
    /**
     * Authenticate the user.
     * @param string $u username
     * @param string $p password
     * @return bool zero for fail, one for succeed
     */
    
    function Auth($u, $p) 
      {
        global $debug;
	if (strstr($u, '@')) {
        $q = "select * from users where email='$u'and passwd=PASSWORD('$p') AND status > 0";
	} else {
        $q = "select * from users where username='$u'and passwd=PASSWORD('$p') AND status > 0";
	}
        $debug->line($q, 5);
        $this->db->query($q, "AuthPiece");
        if ($this->db->Size("AuthPiece") > 0) 
          {
            return 1;
          }
        else
          {
            return 0;
          }
      }
    /**
     * Use the browser's HTTPAuth mechanism for authentication
     * @param string $html [optional] String of html for displaying if authentication fails.
     * @param string $realm [optional] Authentication realm
     * @return bool zero for fail, one for succeed
     */
    
    function HTTPAuth($html = "Authorization Required", $realm = "Private") 
      {
        global $AGLOBAL, $debug;
        $val = 0;
        global $PHP_AUTH_USER, $PHP_AUTH_PW;
        if (isset($PHP_AUTH_USER) && isset($PHP_AUTH_PW)) 
          {
            $val = $this->Auth($PHP_AUTH_USER, $PHP_AUTH_PW);
          }
        if ($val == 0) 
          {
            header('WWW-Authenticate: Basic realm="' . $realm . '"');
            header('HTTP/1.0 401 Unauthorized');
            echo $html;
            exit;
          }
        return $val;
      }
    /**
     * Logout someone who authenticated using cookies.
     */
    
    function CookieLogout() 
      {
        global $AGLOBAL, $debug;
        $debug->line('Cookie Logout Called', 5);
        global $_COOKIE, $AGLOBAL;
        $u = $_COOKIE[$AGLOBAL['cookieuser_idnum']];
        $t = $_COOKIE[$AGLOBAL['cookieUserToken']];
        setcookie($AGLOBAL['cookieUserToken'], "", time() -3600, $AGLOBAL['USERCOOKIEPATH'], $AGLOBAL['USERCOOKIEDOMAIN'], $AGLOBAL['USERCOOKIESECURE']);
        setcookie($AGLOBAL['cookieuser_idnum'], "", time() -3600, $AGLOBAL['USERCOOKIEPATH'], $AGLOBAL['USERCOOKIEDOMAIN'], $AGLOBAL['USERCOOKIESECURE']);
        unset($_COOKIE[$AGLOBAL['cookieuser_idnum']]);
        unset($_COOKIE[$AGLOBAL['cookieUserToken']]);
      }
    /**
     * Authenticate someone using cookies
     * If called with no args, checks to see if there's a valid
     * cookie. If called with username and password, it attempts to
     * authenticate with {@link user::Auth()} and if successful sets a
     * cookie.
     * @param string $username [optional] Username
     * @param string $password [optional] Password
     * @return bool zero for failure, one for success. (On success,
     * <code>$this->uid</code> and <code>$this->loggedIn</code> are
     * set appropriately.
     */
    
    function CookieAuth() 
      {
        global $AGLOBAL, $debug;
        $debug->line('Cookie Auth Called', 5);
        #Called with no args, checks to see if there's a valid cookie.
        # Return 1 if valid cookie, otherwise, 0. Also sets $user->uid to cookie id and $user->loggedIn to 1;
        # Called with a username/password, does auth & sets cookie.
        global $_COOKIE, $AGLOBAL;
        $u = $_COOKIE[$AGLOBAL['cookieuser_idnum']];
        $t = $_COOKIE[$AGLOBAL['cookieUserToken']];
        $debug->line("Got $u $t", 5);
        $debug->variable($_COOKIE, 5);
        if (func_num_args() > 0) 
          {
            $debug->line('func_num_args > 0', 5);
            $args = func_get_args();
            $debug->variable($args, 5);
            $debug->line("Got args", 5);
            $u = $args[0];
            $p = $args[1];
            if ($this->Auth($u, $p)) 
              {
                $q = "select passwd from users where username='$u' AND status >0";
                $debug->line('Succeeded auth, Query: ' . $q, 5);
                $this->db->query($q, 'cookieauth');
                $row = $this->db->next_record('cookieauth');
                $h = $row['passwd'];
                $debug->line('h is ' . $h, 5);
                $token = md5($u . $h . $AGLOBAL['USERCOOKIESALT']);
                $debug->line('token is ' . $token, 5);
                setcookie($AGLOBAL['cookieUserToken'], $token, $AGLOBAL['USERCOOKIEEXPIRE'], $AGLOBAL['USERCOOKIEPATH'], $AGLOBAL['USERCOOKIEDOMAIN'], $AGLOBAL['USERCOOKIESECURE']);
                setcookie($AGLOBAL['cookieuser_idnum'], $u, $AGLOBAL['USERCOOKIEEXPIRE'], $AGLOBAL['USERCOOKIEPATH'], $AGLOBAL['USERCOOKIEDOMAIN'], $AGLOBAL['USERCOOKIESECURE']);
                $this->uid = $u;
                $this->loggedIn = 1;
                return 1;
              }
            else
              {
                $debug->line("Failed Cookie Auth, Query: " . $q, 5);
                $this->loggedIn = 0;
                return 0;
              }
          }
        else
          {
            if (isset($_COOKIE[$AGLOBAL['cookieuser_idnum']]) && isset($_COOKIE[$AGLOBAL['cookieUserToken']])) 
              {
                $debug->line('Want username ' . $u, 5);
                $debug->line('Want token ' . $t, 5);
                $q = "select passwd from users where username='$u' AND status >0";
                #This is a hashed password. We don't care.
                $this->db->query($q, 'cookieauth');
                $row = $this->db->next_record('cookieauth');
                $h = $row['passwd'];
                $debug->line('h is ' . $h, 5);
                $debug->line('want ' . md5($u . $h . $AGLOBAL['USERCOOKIESALT']) , 5);
                if ($t == md5($u . $h . $AGLOBAL['USERCOOKIESALT'])) 
                  {
                    $debug->line('Auth succeeded', 5);
                    $this->uid = $u;
                    $this->loggedIn = 1;
                    return 1;
                  }
                else
                  {
                    $debug->line('auth failed', 5);
                    $this->loggedIn = 0;
                    return 0;
                  }
              }
            else
              {
                $this->loggedIn = 0;
                return 0;
              }
          }
      }
    /**
     * Returns true if the current user is an admin for the application.
     * @param string $application Name of the application or 'all'
     * @return bool zero for no, one for yes.
     *
     */
    
    function isAdmin($application = 'all') 
      {
        $retval = FALSE;
        if (!$this->_isloaded) 
          {
            $this->_loaduser();
          }
        $sql = "select count(*) as total from admin where user_idnum = " . $this->userData['user_idnum'] . " AND application = '$application' AND active = 1";
        $this->db->query($sql, 'isAdmin');
        $res = $this->db->next_record('isAdmin');
        if ($res['total'] > 0) 
          {
            $retval = TRUE;
          }
        return $retval;
      }
    /**
     * Returns contents of userData array.
     */
    
    function getUserData() 
      {
        $this->_loadUser();
        $temp = $this->userData;
        unset($temp['extraData']);
        $uid = $temp['uid'];
        unset($temp['uid']);
        unset($temp['passwd']);
        return $temp;
      }
  }
?>
