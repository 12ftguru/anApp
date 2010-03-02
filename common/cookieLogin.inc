<?PHP
/**
 * cookieLogin provides a method to authenticate someone based on cookies (or alternatively form input)
 */

class cookieLogin extends loginPrototype {
  public function __construct() {
    global $user, $config;
    // Set us up some vars: 
    $loginSafety = ($config->loginSafetyLevel)?$config->loginSafetyLevel:2; //iform level
    $cookieUser = ($config->cookieUserKey)?$config->cookieUserKey:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieTokenKey:$config->appName.'Token';
    

    $form = new iForm($loginSafety); //Strict Form
    $form->addPost(); //We only want posted variables.
    $form->addCookie(); // And cookie vars.
    $form->filter->username( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->password( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->$cookieUser( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->$cookieToken( FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP );
    $form->filter->logout( FILTER_VALIDATE_BOOLEAN );

    // Now we check to see what's going on. First, did we just logout?
    // If so, remove the cookies then reload the page.
    if ($form->logout) {
      $this->CookieLogout($cookieUser,$cookieToken);
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
      // Not logged in and not trying to log in.
      $this->loggedIn = FALSE;
      $this->error = '';
    }
  }

  public function FormAuth($form) {
    global $config, $user, $db;
    $cookieExpiry = ($config->loginExpires)?$config->loginExpires:(8*60*60); //default 8 hours from now.
    $cookieUser = ($config->cookieUserKey)?$config->cookieUserKey:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieTokenKey:$config->appName.'Token';
    $cookiePath = ($config->cookiePath)?$config->cookiePath:"";
    $cookieDomain = ($config->cookieDomain)?$config->cookieDomain:"";
    $cookieSecure = ($config->cookieSecure)?$config->cookieSecure:FALSE;
    $salt = ($config->cookieSalt)?$config->cookieSalt:'anAppLoginCookieSalt';

    $sql = " (`loginName` = '".$form->username."' OR `email` = '".$form->username."') AND `passwd` = PASSWORD('".$form->password."')";
    $factory = smartObjectFactory::getInstance($db);
    $users = $factory->iuser()->where($sql)->f();
    if ($users->num_rows == 1) {
      D::log("0 User object:", "LOG", "CookieLogin");
      $user = $users[0];
      D::variable($user, "VARIABLE", "CookieLogin");
      D::log("0 User resultset:", "LOG", "CookieLogin");
      D::variable($users, "VARIABLE", "CookieLogin");
      
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
    global $config, $user, $db;
    $cookieExpiry = ($config->loginExpires)?$config->loginExpires:(8*60*60); //default 8 hours from now.
    $cookieUser = ($config->cookieUserKey)?$config->cookieUserKey:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieTokenKey:$config->appName.'Token';
    $cookiePath = ($config->cookiePath)?$config->cookiePath:"";
    $cookieDomain = ($config->cookieDomain)?$config->cookieDomain:"";
    $cookieSecure = ($config->cookieSecure)?$config->cookieSecure:FALSE;
    $salt = ($config->cookieSalt)?$config->cookieSalt:'anAppLoginCookieSalt';
    $sql = " (`loginName` = '".$form->$cookieUser."' OR `email` = '".$form->$cookieUser."') AND MD5(CONCAT('".$form->$cookieUser."', `passwd`, '".$salt."')) = '".$form->$cookieToken."'";
    $factory = smartObjectFactory::getInstance($db);
    $users = $factory->iuser()->where($sql)->f();
    if ($users->num_rows == 1) {
    	D::log("1 User object:", "LOG", "CookieLogin");
      $user = $users[0];
      D::variable($user, "VARIABLE", "CookieLogin");
      D::log("1 User resultset:", "LOG", "CookieLogin");
      D::variable($users, "VARIABLE", "CookieLogin");
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
    $cookieUser = ($config->cookieUserKey)?$config->cookieUserKey:$config->appName.'User';
    $cookieToken = ($config->cookieTokenKey)?$config->cookieTokenKey:$config->appName.'Token';
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