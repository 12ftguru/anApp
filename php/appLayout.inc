<?PHP

/**
  * Our default applayout class.
  *
  * @package anApp
  */
class appLayout extends aWidget
{

   /**
     * our constructor
     *
     * @return string
     */
    function __construct() {
        global $debug, $db, $user;
	$retval = array('success' => TRUE);
	$retval['appLayout'] = array(
				     'layout' => 'border',
				     'items' => array(
						      array(
							    'region' => 'center',
							    'html' => 'Center Region',
							    'id' => 'centerDesktop'
							    ),
						      array(
							    'region' => 'north',
							    'html' => 'anApp 3.0 test app',
							    'collapsible' => TRUE
							    ),
						      array(
							    'region' => 'east',
							    'html' => 'Here',
							    'title' => 'East',
							    'id' => 'eastpanel',
							    'collapsible' => TRUE,
							    'width' => 300,
							    'tbar' => array('action:anApp.testButton1', 'action:anApp.testButton2', 'action:anApp.testButton3')

							    )
						      )
				     );
	$this->json($retval);

    }



}



?>