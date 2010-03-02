<?PHP


abstract class loginPrototype extends anObject {
  public $loggedIn;
  public $error;

}

class dummyLogin extends loginPrototype {
  
  public function __construct() {
    global $user;
    $user = new iuser(array(
			    'userID' => 0,
			    'loginName' => 'none',
			    'gn' => 'Dummy',
			    'sn' => 'User',
			    'access' => 1,
			    'status' => 1
			    )); //fake user.
    $this->loggedIn = TRUE;
    $this->error = 'Dummy User.';
  }
}


?>