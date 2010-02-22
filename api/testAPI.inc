<?PHP
class testAPI {

  /**
   * @remotable
   * @formHandler
   */
  public function formPost($post) {
    $retval = array(
      'success' => TRUE,
      'post' => $post
    );
    return $retval;
  }

  /**
   * @remotable
   */
  public function directRead($smartObject, $where = '1') {
    public $db, $config, $user;
    if ($user->access == ANAPPADMIN) {
      $f = smartObjectFactory::getInstance($db);
      $res = $f->$smartObject()->where($where);
      $data = array();
      while ($row = $res->fetch()) {
        $data[] = $row->vars;
      }
      return array(
        'success' => TRUE,
        'data' => $data, 
        'args' => array(
          'smartObject' => $smartObject, 
          'where' => $where
        )
      );
    } else {
      return array('success' => FALSE, 'error' => "Not allowed.");
    }
  }

  /**
   * @remotable
   */
  public function directEcho($data) {
    return $data;
  }

  /**
   * @remotable
   * @formHandler
   */
  public function uploadFile($post) {
    $upload = new upload();

    if ($post['allowedType']) {
      $allowed = $post['allowedType'];
      unset($post['allowedType']);
    } else {
      $allowed = '';
    }
    
    if ($upload->verify('uploadedfile',$allowed)) {
      $retval = array(
        'success' => TRUE,
        'fileInfo' => $upload->file,
        'formInfo' => $post
      );
    } else {
      $retval = array(
        'success' => FALSE,
        'errors' => $upload->errors
      );
    }

    unlink($upload->file["tmp_name"]);
    return $retval;
  }


}

?>