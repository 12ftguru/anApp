<?php
/**
 * idb -- Improved DB
 *      This is a simple extension of the mysqli object that allows
 * for dsn based connections as well as arguments based.
 *
 * @author BrYan P. Johnson
 * @package aLib
 * @version 1.0
 */
class idb extends mysqli
  {
    public $user;
    public $pass;
    public $db;
    public $host;
    
    function __construct() 
      {
        $nargs = func_num_args();
        $args = func_get_args();
        if ($nargs == 1) 
          {
            if (is_array($args[0])) 
              {
                $this->user = $args[0]['user'];
                $this->pass = $args[0]['password'];
                $this->db = $args[0]['db'];
                $this->host = $args[0]['host'];
              }
            else
              {
                if (preg_match("#mysql:\/\/([^:]+):([^@]+)\@([^\/]+)\/(.*)#", $args[0], $matches)) 
                  {
                    $this->user = $matches[1];
                    $this->pass = $matches[2];
                    $this->db = $matches[4];
                    $this->host = $matches[3];
                  }
              }
          }
        else
          {
            $this->user = $args[0];
            $this->pass = $args[1];
            $this->db = $args[3];
            $this->host = $args[2];
          }
        parent::__construct($this->host, $this->user, $this->pass, $this->db);
      }
    
    function __destruct() 
      {
        $this->close();
      }
    /**
     * f
     *
     */
    
    function f() 
      {
        print ("F");
      }
  };
?>
