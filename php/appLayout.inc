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
                                     'height' => 550,
                                     'width' => 700,
				     'items' => array(
						      array(
							    'region' => 'center',
							    'html' => 'Center Region',
							    'id' => 'centerDesktop'
							    ),
						      array(
							    'region' => 'east',
							    'html' => 'Here',
							    'title' => 'East',
							    'id' => 'eastpanel',
							    'collapsible' => TRUE,
							    'width' => 300
							    )
						      )
				     );
	$this->json($retval);

    }



}



?>