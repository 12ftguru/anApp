<?PHP
/**
 * cookieLogin provides a method to authenticate someone based on cookies (or alternatively form input)
 */

class cookieLogin extends loginPrototype {
  public function __construct() {
    global $user, $debug, $config;
    // Set us up some vars: 
    $loginSafety = ($config->loginSafetyLevel)?$config->loginSafetyLevel:2; //iform level
    $cookieUser = ($config->cookieUserKey)?$config->cookieUser:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieToken:$config->appName.'Token';
    

    $form = new iForm($loginSafety); //Strict Form
    $form->addPost(); //We only want posted variables.
    $form->addCookie(); // And cookie vars.
    $form->filter->username( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->password( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->$cookieUser( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->$cookieToken( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->logout( FILTER_VALIDATE_BOOLEAN );

    $debug->debug('Performing Auth', 5);
    $debug->debug($form,5);
      $debug->debug($_COOKIE,5);
    // Now we check to see what's going on. First, did we just logout?
    // If so, remove the cookies then reload the page.
    if ($form->logout) {
      $debug->debug('Logging out',5);
      $this->CookieLogout($cookieUser,$cookieToken);
      $debug->level = 0;
      $reloadURL = ($config->logoutURL)?$config->logoutURL:$config->appURL;
      header("Location: ".$reloadURL);
      exit;
    }
    // Next, do we have valid cookies?
    if ($form->$cookieUser !== FALSE && $form->$cookieToken !== FALSE) {
      $this->CookieAuth($form);
    } elseif ($form->username !== FALSE && $form->password !== FALSE) {
      // Else we should check form auth
      $this->FormAuth($form);
    } else {
      $debug->debug('No Auth:', 5);
      // Not logged in and not trying to log in.
      $this->loggedIn = FALSE;
      $this->error = '';
    }
  }

  public function FormAuth($form) {
    global $config, $user, $debug;
    $cookieExpiry = ($config->loginExpires)?$config->loginExpires:(8*60*60); //default 8 hours from now.
    $cookieUser = ($config->cookieUserKey)?$config->cookieUser:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieToken:$config->appName.'Token';
    $cookiePath = ($config->cookiePath)?$config->cookiePath:"";
    $cookieDomain = ($config->cookieDomain)?$config->cookieDomain:"";
    $cookieSecure = ($config->cookieSecure)?$config->cookieSecure:FALSE;
    $salt = ($config->cookieSalt)?$config->cookieSalt:'anAppLoginCookieSalt';

    $sql = "select * from `users` where (`loginName` = '".$form->username."' OR `email` = '".$form->username."') AND `passwd` = PASSWORD('".$form->password."')";
    $debug->debug('Cookie Auth User SQL: '.$sql, 6);
    $users = iuserFactory::query($sql);
    if ($users->num_rows == 1) {
      $user = $users->fetch();
      $this->loggedIn = TRUE;
      //Set cookies:
      $passwordHash = md5($form->username.$user->passwd.$salt);
      setcookie($cookieUser, $form->username, time() + $cookieExpiry, $cookiePath, $cookieDomain, $cookieSecure);
      setcookie($cookieToken, $passwordHash, time() + $cookieExpiry , $cookiePath, $cookieDomain, $cookieSecure);
      //And reload the page:
      $reloadURL = ($config->loginURL)?$config->loginURL:$config->appURL;
      header("Location: ".$reloadURL);
    } else {
      $this->loggedIn = FALSE;
      $this->error = 'Invalid cookie information: '.$form->$cookieUser.' '.$form->$cookieToken;
    }




    return $this->loggedIn;
  }

  public function CookieAuth($form) {
    global $config, $user, $debug;
    $cookieExpiry = ($config->loginExpires)?$config->loginExpires:(8*60*60); //default 8 hours from now.
    $cookieUser = ($config->cookieUserKey)?$config->cookieUser:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieToken:$config->appName.'Token';
    $cookiePath = ($config->cookiePath)?$config->cookiePath:"";
    $cookieDomain = ($config->cookieDomain)?$config->cookieDomain:"";
    $cookieSecure = ($config->cookieSecure)?$config->cookieSecure:FALSE;
    $salt = ($config->cookieSalt)?$config->cookieSalt:'anAppLoginCookieSalt';
    $sql = "select * from `users` where (`loginName` = '".$form->$cookieUser."' OR `email` = '".$form->$cookieUser."') AND MD5(CONCAT('".$form->$cookieUser."', `passwd`, '".$salt."')) = '".$form->$cookieToken."'";
    $debug->debug('Cookie Auth User SQL: '.$sql, 6);
    $users = iuserFactory::query($sql);
    if ($users->num_rows == 1) {
      $user = $users->fetch();
      $this->loggedIn = TRUE;
      //Update session expiry:
      setcookie($cookieUser, $form->$cookieUser, time() + $cookieExpiry, $cookiePath, $cookieDomain, $cookieSecure);
      setcookie($cookieToken, $form->$cookieToken, time() + $cookieExpiry , $cookiePath, $cookieDomain, $cookieSecure);
    } else {
      $this->loggedIn = FALSE;
      $this->error = 'Invalid cookie information: '.$form->$cookieUser.' '.$form->$cookieToken;
    }
    return $this->loggedIn;
  }


  public function CookieLogout($cookieUser, $cookieToken) {
    global $config;
    $cookieUser = ($config->cookieUserKey)?$config->cookieUser:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieToken:$config->appName.'Token';
    $cookiePath = ($config->cookiePath)?$config->cookiePath:"";
    $cookieDomain = ($config->cookieDomain)?$config->cookieDomain:"";
    $cookieSecure = ($config->cookieSecure)?$config->cookieSecure:FALSE;
    setcookie($cookieUser, "", time() -3600, $cookiePath, $cookieDomain, $cookieSecure);
    setcookie($cookieToken, "", time() -3600, $cookiePath, $cookieDomain, $cookieSecure);
    unset($_COOKIE[$cookieUser]);
    unset($_COOKIE[$cookieToken]);
  }

}

?>