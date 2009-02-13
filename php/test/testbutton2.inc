<?PHP


class testSmart1 extends smartObject
{
  public $readSQL = 'select * from items';
  public $insertSQL = array(
		     'sql' => 'insert into items set streamID = ?, title = ?, description = ?, content = ?',
		     'types' => 'isss',
		     'vars' => array('streamID', 'title', 'title', 'content')
		     );
  public $deleteSQL = 'delete from items';
  public $updateSQL = array(
		     'sql' => 'update items set title = ?, content = ? where itemID = ?',
		     'types' => 'ssi',
		     'vars' => array('title', 'content', 'itemID')
		     );
  public $idField = 'itemID';
  public $foreignKeys = array('stream' => array('key' => 'streamID',
					 'type' => 'testSmart2'));
  public $format = array(
		  'string' => 'ITEM %d: %s',
		  'vars' => array('itemID', 'title')
		  );
}

class testSmart2 extends smartObject
{
  public $readSQL = 'select * from streams';
  public $insertSQL = array(
		     'sql' => 'insert into streams set private = ?, description = ?',
		     'types' => 'is',
		     'vars' => array('streamID', 'content')
		     );
  public $deleteSQL = 'delete from streams';
  public $updateSQL = array(
		     'sql' => 'update streams set private = ?, description = ? where streamID = ?',
		     'types' => 'isi',
		     'vars' => array('streamID', 'content', 'streamID')
		     );
  public $idField = 'streamID';
  public $foreignKeys = array('stream' => array('key' => 'streamID',
					 'type' => 'testSmart2'));
  public $format = array(
		  'string' => 'STREAM %d: %s',
		  'vars' => array('streamID', 'description')
		  );
}



/**
  * Our default applayout class.
  *
  * @package anApp
  */
class testbutton2 extends aWidget
{

  public $attemptedWidget = 'testbutton2';

   /**
     * our constructor
     *
     * @return string
     */
    function __construct($newName) {
        global $debug, $db, $user;
	$debug->level = 3;
	$this->attemptedWidget = ($newName)?$newName:$this->attemptedWidget;
	$testItem1 = new testSmart1(1);

        $testItem1->title = 'Test me!';
	if ($testItem1->update() === FALSE) {
	  $debug->debug('Update error: '.$testItem1->errno.' '.$testItem1->error, 1);
	}


	$testStream2 = new testSmart2(2);
	$debug->debug($testItem1,1);
	  $debug->debug($testStream2, 1);
	    $debug->debug($testItem1->stream->title, 1);

	    
	    $debug->debug($this, 1);

	$retval = array('success' => TRUE);
	$retval['create'][] = array(
				    'xtype' => 'error',
				    'title' => 'Unknown Error: '.$this->attemptedWidget,
				    'msg' => "There has been an unexpected error. Since it's unexpected, we aren't really sure what to tell you. Maybe you ought to call someone.<br />".$testItem1,
				    'hidden' => FALSE,
				    'id' => 'errorWindow',
				    'autoShow' => TRUE
				    );
	$this->json($retval);


    }



}



?>