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
        global $debug, $db;
	$html = "";

	$user3 = new iuser();
	$user3->gn = "John";
	$user3->sn = "Clark";
	$user3->loginName = "john";
	$user3->email = "john@12ftguru.com";
	$user3->access = 0;
	$user3->status = 1;
	$user3->passwd = "foo";
	$user3->create();

	$res = iuserFactory::query('select * from users');
	$debug->debug('Res: ',2);
	$debug->debug($res, 2);

	while ($user = $res->fetch()) {
	  $html .= htmlutf8entities($user) . "<br />";
	}

	$user2 = new iuser(1);



	$retval = array('success' => TRUE);
	$retval['update']['eastpanel'] = array(
					       'border' => FALSE,
					       'setTitle' => array('Title Changed'),
					       'collapse' => array()
				     );
	$retval['create'][] = array(
				    'xtype' => 'notification',
				    'title' => 'Primary User: '.$user2,
				    'msg' => $html,
				    'id' => 'systemMessageWindow',
				    'autoShow' => TRUE,
				    'buttons' => 'action:Ext.Msg.OK',
				    );
	$this->json($retval);


    }



}



?>