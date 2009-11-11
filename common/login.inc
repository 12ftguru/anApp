<?PHP


abstract class loginPrototype extends anObject {
  public $loggedIn;
  public $error;

}

class dummyLogin extends loginPrototype {
  
  public function __construct() {
    global $user, $debug;
    $user = new iuser(array(
			    'userID' => 0,
			    'loginName' => 'none',
			    'gn' => 'Dummy',
			    'sn' => 'User',
			    'access' => 1,
			    'status' => 1
			    )); //fake user.
    $debug->debug('New User',6);
    $debug->debug($user, 6);
    $this->loggedIn = TRUE;
    $this->error = 'Dummy User.';
  }
}


?>
