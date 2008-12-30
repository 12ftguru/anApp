<?PHP

class User_Exception extends Exception { };
class UserDBConnection_Exception extends User_Exception { };
class UserQuery_Exception extends User_Exception { };

class userDB extends MySQLi
{
 
    protected static $instance;
 
    protected static $options = array();

    /*
      A typical options array has the following:
      
      userDB => mysql://user:pass@host/db url for connecting to the db
        or
      host, user, pass, dbname
      
      searchFields => array('column1', 'column2') default fields to search

      id => 'idColumn' column containing the unique user id
      access => 'accessColumn' column containing the access level
      status => 'statusColumn' column containing the account status bit (used by login plugins)

      loginPlugins => array('plugin1', 'plugin2') Login plugins to query, in order, when the login function is called.


      Also, if you don't set any options, we will use the values in $config->userDBOptions

     */
 
    private function __construct() {
      global $config;
      /* Set options to config->userDBOptions */ 
        $o = self::$options;

	if (count(array_keys($o)) == 0) {
	  $o = $config->userDBOptions;
	  self::setOptions($o);
	}

	if ($o['userDB'] && preg_match("#mysql:\/\/([^:]+):([^@]+)\@([^\/]+)\/(.*)#", $o['userDB'], $matches)) {
	  $o['user'] = $matches[1];
	  $o['pass'] = $matches[2];
	  $o['db'] = $matches[4];
	  $o['host'] = $matches[3];
	  self::setOptions($o);
	} else if ($config->userDB  && preg_match("#mysql:\/\/([^:]+):([^@]+)\@([^\/]+)\/(.*)#", $config->userDB, $matches)) {
	  $o['userDB'] = $config->userDB;
	  $o['user'] = $matches[1];
	  $o['pass'] = $matches[2];
	  $o['db'] = $matches[4];
	  $o['host'] = $matches[3];
	  self::setOptions($o);
	}
        // turn of error reporting
        mysqli_report(MYSQLI_REPORT_OFF);
        // connect to database (@ means don't break on errors, just
        // move along, we'll catch the errors later)
        @parent::__construct(isset($o['host'])   ? $o['host']   : 'localhost',
                             isset($o['user'])   ? $o['user']   : 'root',
                             isset($o['pass'])   ? $o['pass']   : '',
                             isset($o['dbname']) ? $o['dbname'] : 'test',
                             isset($o['port'])   ? $o['port']   : 3306,
                             isset($o['sock'])   ? $o['sock']   : false );
 
        // check if a connection established
        if( mysqli_connect_errno() ) {
            throw new UserConnection_Exception(mysqli_connect_error(), mysqli_connect_errno()); 
        }
    }

    public static function getUser ($identifier, $searchField = NULL) {
      global $debug;
      if (!self::$instance) {
	self::$instance = new self();
      }
      /* Query single user here. */
      $sql = 'select * from users where ';
      if ($searchField) {
	$sql .= "`".$searchField."` = '".$identifier."'";
      } else {
	$o = self::$options;
	foreach ($o['searchFields'] as $searchField) {
	  $sql .= "`".$searchField."` = '".$identifier."' AND ";
	}
	$sql = substr($sql, 0, -4);
      }
      $debug->debug('Getting user with '.$sql, 9);
	$res = $this->query($sql);
      if (is_object($res)) {
	if ($res->num_rows == 1) {
	  $retval = $res->fetch_object('user');
	  $res->free();
	    return $retval;
	} else {
	  return FALSE;
	}
      } else {
	throw new UserQuery_Exception(mysqli_error(), mysqli_errno()); 
      }
      // Shouldn't make it this far.
      return FALSE;
    } 
 
    public static function searchUsers ($identifier, $searchField = NULL) {
      if (!self::$instance) {
	self::$instance = new self();
      }
      /* Query multiple users here. */
      

    } 

    public static function login () {
      if (!self::$instance) {
	self::$instance = new self();
      }
      /* Do plugin based login here. */
      

    }

    public static function getInstance() {
 
        if( !self::$instance ) {
 
            self::$instance = new self(); 
        }
        return self::$instance;
    }
 
 
    public static function setOptions( array $opt ) {
 
        self::$options = array_merge(self::$options, $opt);
 
    }
 
    public function query($query) {
 
        if( !$this->real_query($query) ) {
 
            throw new UserQuery_Exception( $this->error, $this->errno );
        }
 
        $result = new UserQuery($this);
        return $result;
 
    }
 
 
    public function prepare($query) {
 
        $stmt = new UserStmt($this, $query);
        return $stmt;
 
    }    
}

/* Need to extend mysqli_result with UserQuery, mysqli_stmt with UserStmt and create a user object that is called by UserQuery->fetch_object */



?>