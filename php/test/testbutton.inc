<?PHP

/**
  * Our default applayout class.
  *
  * @package anApp
  */
class testbutton extends aWidget
{

   /**
     * our constructor
     *
     * @return string
     */
    function __construct() {
        global $debug, $db, $user;
	$debug->level = 3;
	$retval = array('success' => TRUE);
	$retval['update']['eastpanel'] = array(
					       'border' => FALSE,
					       'setTitle' => array('Title Changed'),
					       'collapse' => array()
				     );
	$retval['create'][] = array(
				    'xtype' => 'notification',
				    'title' => 'System Notification',
				    'html' => "This is a test. This is only a test.",
				    'hidden' => FALSE,
				    'id' => 'systemMessageWindow',
				    'delay' => 5000,
				    'autoShow' => TRUE
				    );
	$this->json($retval);


    }



}



?>