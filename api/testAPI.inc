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