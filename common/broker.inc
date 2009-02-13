<?php

/* Exception Handling. Your exceptions will appear in Firebug: */

/**
  * Custom Exception Handler
  *
  * @param Exception $e
  * @return none
  */
function brokerExceptionHandler( Exception $e ) {
    global $debug, $user;
    $debug->level = ($debug->level > 1)?$debug->level:1;
    $debug->exception($e);
    if ( $debug->level > 0 && $user->access == ANAPPADMIN) {
        $output = array('success' => TRUE);
        $output['updateItems']['items'][] = array('id' => 'debugGrid',
                                                  'debugRecords' => $debug->dump_anAppJSON());
        $output = json_encode($output);
        $output = preg_replace('/"action:([^\"]+)"/', '$1', $output);
        print $output;
        $debug->level = 0;
        
    }              
    exit;
}

global $login;

if ( $login->loggedIn ) {
    set_exception_handler( 'brokerExceptionHandler' );
}

class BrokerException extends Exception { }
class BrokerMissingWidgetException extends BrokerException { }

/**
  * Broker decides, based on our url, what file to display. Existing
  *  images and html files are automatically included, and widgets are
  *  simply included. It's then up to the widgets to decide what they
  *  display.
  *
  * @package anApp
  */
class broker extends anObject
{
    private $forbiddenExtensionsRegex = "/\.(inc|php|phps|php5|ihtml|js)$/";


   /**
     * Create the broker.
     *
     * @return none
     */
    function __construct() {
        /* First see if our file exists and isn't in the excluded array. */
            global $user, $config, $debug, $db, $login;
            if ( ! is_object($db) ) {
                $db = new idb($config->mainDB);                
            }
            
        list($uri, $cruft) = explode('?', $_SERVER['REQUEST_URI']);

        $fileInfo = explode('/', rtrim($uri, '/'));
        $debug->debug('Checking url %s %s', $uri, $fileInfo, 3);

	    if ( $uri == $config->self ) {
                if ( $debug->level == 0 && $user->access == ANAPPADMIN && !$config->deployed) {
                    $debug->level = 1;
                    $debug->line('anApp loaded, admin user detected, dev environment detected.',1);
                    $debug->showFiles(1);
                    
                }
                $debug->debug('Requested index, serving template %s', $config->loggedInTemplate,2);
                $template = new template($config->loggedInTemplate);
                $template->set('config',$config);
                $template->set('user',$user);
                $template->set('title', $config->defaultTitle);
                $template->set('appName', $config->appName);
                $template->set('extLocation', $config->extLocation);
                $template->set('additionalJS', $config->javascripts);
                $template->set('additionalCSS', $config->styles);
                $template->display();
                exit;
       
            }

        if(file_exists($_SERVER['DOCUMENT_ROOT'].$uri) and ($_SERVER['SCRIPT_FILENAME']!=$_SERVER['DOCUMENT_ROOT'].$_SERVER['REQUEST_URI']) and ($_SERVER['REQUEST_URI']!="/") and !preg_match($this->forbiddenExtensionsRegex, $_SERVER['REQUEST_URI'])){
            $array = explode("/", $_SERVER['SCRIPT_NAME']);
            $path1 = $_SERVER['DOCUMENT_ROOT'].$uri."/".$array[count($array)-1];
            $path2 = preg_replace('/\/\//', '/', $path1);
            $path3 = str_replace("\\", '/', __FILE__);
            
            if ($path2 != $path3) {
                list($url,$cruft2)=explode('?',$_SERVER['REQUEST_URI']);
                include($_SERVER['DOCUMENT_ROOT'].$url);
                exit();
            }
        }

        /* Next we see if it's a javascript file. */
        if ( preg_match('/.js$/', $uri) ) {
            /* Get the file name, sans .js */
            list($filename,$extension) = explode('.', $fileInfo[count($fileInfo)-1] );
            if ( file_exists($_SERVER['DOCUMENT_ROOT'].'/'.$filename.'.ijs') ) {

            $template = new template('js/'.$filename.'.ijs');
            global $user, $config;
            $template->set('user',$user);
            $template->set('config',$config);
            $template->set('title', $config->defaultTitle);
            $template->set('appName', $config->appName);
            $template->set('extLocation', $config->extLocation);

            $template->display();
            exit;
                            
            } else {
                $debug->level = -1;
                include( $_SERVER['DOCUMENT_ROOT'].$uri );
                exit();
            }
        }

        /* Check to see if we have a widget... */
            $debug->debug('Checking fileInfo: %s', $uri, 3);
            
        if ( preg_match('/\/[A-Za-z0-9]+(\/)*$/', $uri ) && !preg_match('/permalink\/[A-Za-z]+\/[0-9]+(\/)*$/', $uri)) {
	  try {
            $debug->debug('Including %s',$fileInfo[count($fileInfo)-1],3); 
            $widget = new $fileInfo[count($fileInfo)-1];
            $widget->display();
            exit;
	  } catch (Exception $e) {
	    if ($config->defaultWidget != '') {
	      $widget = new $config->defaultWidget;
	      $widget->display();
	      exit;
	    } else {
	      throw new BrokerMissingWidgetException($e);
	    }
	  }
        }

        /* If we've gotten this far, and we're logged in, just return
         the index page. */
            $template = new template($config->loggedInTemplate);
            $template->set('config',$config);       
            $template->set('user',$user);
            $template->set('title', $config->defaultTitle);
            $template->set('appName', $config->appName);
            $template->set('extLocation', $config->extLocation);
                $template->set('additionalJS', $config->javascripts);
                $template->set('additionalCSS', $config->styles);

        $template->display();
        exit;
        
        
    }
    
};


?>