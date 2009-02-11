<?PHP

/**
 * There are two ways one won't get a true login. One is if you don't attempt to log in, and one is if you do and fail.
 * If $login->loggedIn is false, you're in one of these error states. If $login->error == BADLOGIN, you failed auth.
 */

define("BADLOGIN", 1);


class login extends anObject {
  public $user;
  public $loggedIn;
  public $error;

  function __construct () {
    global $debug, $arguments, $AGLOBAL, $config;
    $AGLOBAL['USERCOOKIEEXPIRE'] = time()+$config->loginExpires; #8 hours from now.
    $AGLOBAL['cookieUserID'] = $config->appName.'User';
    $AGLOBAL['cookieUserToken'] = $config->appName.'Token';

    /* TODO: Fix the User Object so I don't have to do this: */
    $AGLOBAL['USERDBURL'] = $config->userDB;
    

#    $form = new formhandler(2, array('username', 'password'));
    $form = new iForm(2); #Strict Form
    $form->addPost(); #We only want posted variables.
    $form->addCookie();
    
      
    $form->filter->username( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->password( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->logout( FILTER_VALIDATE_BOOLEAN );
    $debug->debug('Form %s %s', $form->username, $form->password, 10);
    $this->user = new iuser();
    $badlogin = 0;
    $this->loggedIn = FALSE;
    if ($form->logout == 1  || !(stristr($_SERVER['REQUEST_URI'], "logout") === FALSE)) {
#	$this->user->trackUsageStats("logout");
	$this->user->CookieLogout();
      $debug->level = 0;
      
      
      print "{success:true, reload:true}\n";
      exit;
      
    }
    if ($this->user->CookieAuth()) {
      $debug->line("Passed cookieauth from cookie", 8);
      $this->user->loggedIn = TRUE;
      $this->error = FALSE;
      $this->loggedIn = TRUE;
    } elseif ($form->username && $form->password) {
       $debug->line("Checking cookieauth for " . $form->username . " : " . $form->password, 8);
	if (! $this->user->CookieAuth($form->username, $form->password)) {
	  $debug->line("failed cookieauth for " . $form->username . " : " . $form->password, 8);
	  $this->error = BADLOGIN;
	} else {
	  $debug->line("Passed cookieauth from form", 8);
	  $this->error = FALSE;
	  $this->user->loggedIn = TRUE;
	  $this->loggedIn = TRUE;
        global $permaID, $permaType;
	  
        $reloadURL = $config->appURL;
        
    if ($permaID > 0) {
	$reloadURL .= 'permalink/'.$permaType.'/'.$permaID;	      
    }
	  $this->user->trackUsageStats("login");
	  
	  #Now we reload the page...
	  
          header("Location: ".$reloadURL);
          exit;
          
	}
    }
    parent::__construct();
  }

}



?>
