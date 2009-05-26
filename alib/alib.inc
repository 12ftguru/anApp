<?php
/**
 * Alib Master File -- just include_once and forget.
 * This file inits several variables. So better check
 * them. (They're documented below). It also defines the anObject
 * object, the one from which all other alib objects should be
 * defined.
 * @package aLib
 * @author BrYan P. Johnson
 * @version 3.0
 */
/**
 * function alib_init -- initializes the alib. Don't worry. Just ignore it.
 */

function alibInit() 
  {
    addIncludePath('.');
    global $debug, $debugLevel, $debugStyle;
    if ($debugStyle == 'firebug') {
      include_once ('debug.fb.inc');
    } else {
      include_once ('debug.inc');
    }
    $debug = Debug::singleton();
    $debug->setEnabled(TRUE);
    $debug->level = ($debugLevel) ? $debugLevel : 0;
    include_once ('configHandler.inc');
  }
/**
 * Add a path to the include path, independent of system
 *
 * @param string or array $directory Directory(s) to add to the include path
 * @param bool $recurse Add subdirectories to the include path
 * @return none
 */

function addIncludePath($directory, $recurse = FALSE) 
  {
    if (is_array($directory)) 
      {
        foreach($directory as $dir) 
          {
            addIncludePath($dir, $recurse);
          }
      }
    else
      {
          
        if (stristr(php_uname('s') , 'win') && !stristr(php_uname('s') , 'dar')) 
          {
            /* We're on Windows */
            $delim = ";";
          }
        else
          {
            $delim = ":";
            /* Pretty much everyone else. */
          }
        set_include_path(get_include_path() . $delim . $directory);
        if ($recurse) 
          {
              
            if ($handle = opendir($directory)) 
              {
                          
                while (false !== ($filename = readdir($handle))) 
                  {
                     
                      
                    if ($filename != '.' && $filename != '..') {
                         if (is_dir($directory.'/'.$filename)) 
                      {
                         
                        addIncludePath($directory.'/'.$filename, $recurse);
                      }
                         }
                  }
              }
          }
      }
  } # End addIncludePath

/**
 * We autoload any object someone tries to load. If there is a file by
 * the same name as the object, we load it. Since object class names
 * are case insensitive, we downcase the filename. No more including!
 */

function __autoload($class_name) 
  {
    if (preg_match('/^[A-Za-z0-9\.\-\_]+$/', $class_name)) {
    if (!include_once (strtolower($class_name) . ".inc")) 
      {
        include_once ($class_name . ".inc");
      }
    }
  }
/** 
 * The aLib abstract standard object.
 *
 * This way we ensure that all of our objects have the same
 * interfaces.
 * @package aLib
 * @subpackage Utility
 * @author BrYan P. Johnson
 * @version 3.0
 */
abstract class anObject
  {
    public 
    function __construct() 
      {
      }
    public 
    function __get($name) 
      {
        global $debug;
        if ($debug->level >= 10) 
          {
            $debug->line("Object (" . get_class($this) . ") __get $name = ", 10);
            $debug->variable($this->$name, 10);
          }
	if ($this->$name) {
        return $this->$name;
	} else {
	  $v = get_class_vars($this);
	  $underscores = strtolower(preg_replace('/([A-Z]{1,1}[A-Za-z]+)/', '_$1', $name));
	  $camelcase = strtolower(preg_replace('/_/', '', $name));
	  if (is_array($v) && in_array($underscores, $v)) {
	    return $this->$underscores;
	  } else if (is_array($v) && in_array($camelcase, $v)) {
	    return $this->$camelcase;
	  } else {
	    return NULL;
	  }
	}
      }
    /**
     * This catches any calls to undefined functions.
     * If you camelcased when the function was underscored ( fetchAssoc versus fetch_assoc ) or vice versa, we will autocorrect.
     * Yum. With debuglevel of 0, it's silent.
     */
    public 
    function __call($f, $a) 
      {
        global $debug;
	$m = get_class_methods($this);
	$c = get_class($this);
            
	// Check to see if we camelcased and meant underscores:

        $underscores = strtolower(preg_replace('/([A-Z]{1,1}[A-Za-z]+)/', '_$1', $f));
	$camelcase = strtolower(preg_replace('/_/', '', $f));


	if (in_array($underscores, $m)) {
	  call_user_func_array(array($this, $underscores), $a);
	} else if (in_array($camelcase, $m)) {
	  call_user_func_array(array($this, $camelcase), $a);
	} else if ($debug->level >= 1) 
          {
            $debug->line("Undefined function call: " . $f . " (or ".$underscores." ) from class " . $c . " with args:", 1);
            $debug->variable($a, 1);
            $debug->line("Actual allowed methods for " . $c . ":", 1);
            $debug->variable($m, 1);
          }
      }
    public 
    function __set($name, $value) 
      {
        global $debug;
        if ($debug->level >= 10) 
          {
            if (isset($this->$name)) 
              {
                $debug->line("Object (" . get_class($this) . ") changing $name. Was:", 10);
                $debug->variable($this->$name, 10);
                $debug->line("Object (" . get_class($this) . ") changing $name. To:", 10);
                $debug->variable($value);
              }
            else
              {
                $debug->line("Object (" . get_class($this) . ") setting " . $name, 10);
                $debug->variable($value, 10);
              }
          }
        $this->$name = $value;
      }
  }


/**
 * Smart Objects
 *
 * Smart objects are objects tied directly to a single record in a db.
 */

abstract class smartObject extends anObject {
  protected $readSQL = ''; // SQL Goes here
  protected $insertSQL = array(
			     'sql' => '', // statement
			     'types' => '', // types
			   'vars' => array() // Class variables 
			   );
  protected $updateSQL = array(
			     'sql' => '', // statement
			     'types' => '', // types
			   'vars' => array() // Class variables 
			   );
  protected $deleteSQL = '';
  protected $idField = FALSE;
  protected $db;
  protected static $statementCache = array();
  protected $lastResult = array();
  public $error;
  public $errno;
  public $insertID;
  protected $foreignKeys = array();
  protected $serializedKeys = array();
  protected $format = array(
			  'string' => '', // a sprintf style format string
			  'vars' => array() // class variables;
			  );


  public function __construct($id = NULL) {
    global $db, $debug;
    if ($this->idField === FALSE) {
      $this->idField = get_class($this)."ID";
    }
    if (is_a($db, 'idb')) {
      $debug->debug('Initializing', 1);
      $this->db = $db;
      $this->init();
    }
    if (is_array($id)) {
      foreach ($id as $key => $val) {
	$this->{$key} = $val;
      }
    } else if ($id) {
      $this->{$this->idField} = $id;
      $this->read();
    }
  }

    public function __set($name, $value) 
      {
	global $debug;
	if ($name == 'id' && $this->idField != 'id') {
	  $this->{$this->idField} = $value;
	} else if (in_array($name, $this->serializedKeys)) {
	  $this->$name = serialize($value);
	} else {
	  $this->$name = $value;
	}
      }


  public function __get($name) 
      {
        global $debug;
	if ($name == 'id' && $this->idField != 'id') {
	  return $this->{$this->idField};
	} else if (in_array($name, $this->serializedKeys)) {
	  return unserialize(parent::__get($name));
	} else if (!is_array($this->foreignKeys[$name])) {
	  return parent::__get($name);
	} else {
	  if (!is_a($this->foreignKeys[$name]['obj'], 'smartObject')) {
	    $type = $this->foreignKeys[$name]['type'];
	    $fKey = $this->foreignKeys[$name]['key'];
	    $this->foreignKeys[$name]['obj'] = new $type($this->{$fKey});
	  }
	  return $this->foreignKeys[$name]['obj'];
	}
      }

  public function __toString() {
    $vars = array($this->format['string']);
    foreach ($this->format['vars'] as $var) {
      $vars[] = $this->{$var};
    }
    return call_user_func_array('sprintf', $vars);
  }

  public function init() {
        global $debug;
    if (is_a($this->db, 'idb')) {
      $myClass = get_class($this);
      $debug->debug('My Class: '.$myClass, 1);
	if (!is_array(self::$statementCache[$myClass])) {
	// Set up our prepared statements:
	self::$statementCache[$myClass]['insert'] = $this->db->prepare($this->insertSQL['sql']);
	self::$statementCache[$myClass]['update'] = $this->db->prepare($this->updateSQL['sql']);
      }
	if (stristr($this->readSQL, 'where') && !stristr($this->readSQL, $this->idField)) {
	  $this->readSQL .= ' AND `'.$this->idField."` = '-#id#-'";
	} else if (!stristr($this->readSQL, 'where')) {
	  $this->readSQL .= ' WHERE `'.$this->idField."` = '-#id#-'";
	}
	if (stristr($this->deleteSQL, 'where') && !stristr($this->deleteSQL, $this->idField)) {
	  $this->deleteSQL .= ' AND `'.$this->idField."` = '-#id#-'";
	} else if (!stristr($this->deleteSQL, 'where')) {
	  $this->deleteSQL .= ' WHERE `'.$this->idField."` = '-#id#-'";
	}
	$debug->debug('Read SQL: '.$this->readSQL, 1);
    } else {
      return FALSE;
    }
  }

  public function reset() {
    foreach ($this->lastResult as $key => $val) {
      $this->{$key} = NULL;
    }
    foreach ($this->foreignKeys as $name => $arr) {
      unset($arr['obj']);
	$arr['obj'] = NULL;
    }
    $this->lastResult = array();
  }


  public function read() {
    global $debug;
    $debug->debug('Reading...',1);
      $debug->debug($this,1);
      if (is_a($this->db, 'idb')) {
	$this->reset();
	$sql = str_replace('-#id#-', $this->{$this->idField}, $this->readSQL);
	$debug->debug('SmartObject Read SQL: '.$sql, 1);
	$res = $this->db->query($sql);
	if (is_object($res)) {
	  if ($res->num_rows == 1) {
	    $this->lastResult = $res->fetch_assoc();
	    foreach ($this->lastResult as $key => $val) {
	      $this->{$key} = $val;
	    }
	    return TRUE;
	  } else {
	    return FALSE;
	  } 
	} else {
	  return $db->error;
	}
      } else {
	return FALSE;
    }
  }

  public function insert() {
    global $debug;
    if (is_a($this->db, 'idb')) {
      $myClass = get_class($this);
      if (is_a(self::$statementCache[$myClass]['insert'], 'mysqli_stmt')) {
	$insertVals = array($this->insertSQL['types']);
	foreach ($this->insertSQL['vars'] as $var) {
	  if (in_array($var, $this->serializedKeys)) {
	    $insertVals[] = serialize($this->{$var});
	  } else {
	  $insertVals[] = $this->{$var};
	  }
	}
	call_user_func_array(array(self::$statementCache[$myClass]['insert'], 'bind_param'), $insertVals);
	  self::$statementCache[$myClass]['insert']->execute();
	    if (self::$statementCache[$myClass]['insert']->errno > 0) {
	      $this->error = self::$statementCache[$myClass]['insert']->error;
	      $this->errno = self::$statementCache[$myClass]['insert']->errno;
	      $debug->debug('Error: %s %s',self::$statementCache[$myClass]['insert']->errno, self::$statementCache[$myClass]['insert']->error,1);
	      return FALSE;
	    } else {
	      $this->insertID = self::$statementCache[$myClass]['insert']->insert_id;
	      return self::$statementCache[$myClass]['insert']->affected_rows;
	    }
      } else {
	$error = $db->error;
      }

    } else {
      return FALSE;
    }
  }

  public function delete() {
    global $debug;
    if (is_a($this->db, 'idb')) {
      $sql = str_replace('-#id#-', $this->{$this->idField}, $this->deleteSQL);
      $debug->debug('SmartObject Delete SQL: '.$sql, 1);
      $res = $this->db->query($sql);
      $this->reset();
      if (is_object($res)) {
        if ($res->num_rows == 1) {
	  $this->lastResult = $res->fetch_assoc();
	  foreach ($lastResult as $key => $val) {
	    $this->{$key} = $val;
	  }
	} else {
	  return FALSE;
	} 
      } else {
        return $db->error;
      }
    } else {
      return FALSE;
    }
  }

  public function update() {
    global $debug;
    if (is_a($this->db, 'idb')) {
      $myClass = get_class($this);
      if (is_a(self::$statementCache[$myClass]['update'], 'mysqli_stmt')) {
	$updateVals = array($this->updateSQL['types']);
	foreach ($this->updateSQL['vars'] as $var) {
	  if (in_array($var, $this->serializedKeys)) {
	    $updateVals[] = serialize($this->{$var});
	  } else {
	    $updateVals[] = $this->{$var};
	  }
	}
	call_user_func_array(array(self::$statementCache[$myClass]['update'], 'bind_param'), $updateVals);
	  self::$statementCache[$myClass]['update']->execute();
	    if (self::$statementCache[$myClass]['update']->errno > 0) {
	      $this->error = self::$statementCache[$myClass]['update']->error;
	      $this->errno = self::$statementCache[$myClass]['update']->errno;
	      return FALSE;
	    } else {
	      return self::$statementCache[$myClass]['update']->affected_rows;
	    }
      } else {
	$error = $db->error;
      }

    } else {
      return FALSE;
    }
  }


  public function load() {
    return $this->read();
  }
  public function create() {
    return $this->insert();
  }
  public function save() {
    return $this->update();
  }
  public function nuke() {
    return $this->delete();
  }


}

/* You should never have to extend this. Use the factory below. */
class smartResult extends MySQLi_Result {
  public $resultType;
  public $resultParams;

  function fetch($type = NULL, $params = NULL) {
    if (!$type) { $type = $this->resultType; }
    if (!$params) { $params = $this->resultParams; }
    if ($type) {
    return $this->fetch_object($type, $params);
    } else {
      global $debug;
      $debug->debug('sro fetch', 6);
      $debug->debug($this,6);
    }
  }
}


/* Extend this with $objectType and $objectAdditionalParams */
abstract class smartObjectFactory extends anObject {
  public static $objectType;
  public static $objectAdditionalParams;
  private static $instance = NULL;

  public function _query($query, $db = NULL) {
    global $debug;
    if (!$db) { $db = $GLOBALS['db']; };
    if ($db->real_query($query)) {
      $smartResult = new smartResult($db);
      //because static var inheritance can be strange...
      $vars = get_class_vars(get_class($this));
      $smartResult->resultType = $vars['objectType'];
      $smartResult->resultParams = $vars['objectAdditionalParams'];
      $debug->debug('Returning sro '.$class, 6);
      $debug->debug($smartResult, 6);
      return $smartResult;
    } else {
      return FALSE;
      // A real programmer would add exceptions here.
    }
  }

  abstract public static function query ($query, $db = NULL);

  
}

/* Now I do the alibInit because why wouldn't you? And why should I make
you? */
alibInit();
?>
