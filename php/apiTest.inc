<?PHP

class apiTest extends template {

  public function __construct() {
    global $user, $config;
    if ($config->environment == 'dev') {
      $this->file = 'apiTest.ihtml';
      parent::__construct($this->file);
      $this->display();
      exit;
    } else {
      header("HTTP/1.0 404 Not Found");
    }
  }
}
?>