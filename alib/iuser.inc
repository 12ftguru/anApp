<?PHP

/* This file contains two classes, iuser and iuserFactory. Both extend smartObject/soFactory objects.*/

class iuserFactory extends smartObjectFactory {
    public static $objectType = 'iuser';
    public static $objectAdditionalParams = array();

    /* you must include, but feel free to left unchanged, the following: */

    private static $instance;

    public static function query ($query, $db = NULL) {
      if ( self::$instance == NULL ) {
        self::$instance = new self;
      }
      return self::$instance->_query($query, $db);
  }
}


class iuser extends smartObject {
  protected $readSQL = 'select * from users '; // SQL Goes here
  protected $deleteSQL = 'update users set active = 0';
  protected $insertSQL = array(
			     'sql' => 'insert into users set loginName = ?, passwd = PASSWORD(?), gn = ?, sn = ?, email = ?, createDate = UNIX_TIMESTAMP(), modDate = UNIX_TIMESTAMP(), status = ?, access = ?', // statement
			     'types' => 'sssssii', // types
			     'vars' => array('loginName', 'passwd', 'gn', 'sn', 'email', 'access', 'status') // Class variables 
			   );
  protected $updateSQL = array(
			     'sql' => 'update users set loginName = ?, passwd = PASSWORD(?), gn = ?, sn = ?, email = ?, modDate = UNIX_TIMESTAMP(), status = ?, access = ? where userID = ?', // statement
			     'types' => 'sssssiii', // types
			     'vars' => array('loginName', 'passwd', 'gn', 'sn', 'email', 'access', 'status', 'userID') // Class variables 
			   );
  protected $idField = 'userID'; //or FALSE 
  protected $foreignKeys = array(); // '<name>' => array('key' => '<foreignKey>',
  	    		   	       // 	            'type' => '<smartObjectType>),
  protected $serializedKeys = array();
  protected $format = array(
			  'string' => '%s %s <%s>', // a sprintf style format string
			  'vars' => array('gn', 'sn', 'email') // class variables;
			  );  

  function __get($name) {
    if ($name != 'passwd') {
      return parent::__get($name);
    }
  }
  
}

?>