<?php

global $login;


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
      global $user, $config, $db, $login;

        list($uri, $cruft) = explode('?', $_SERVER['REQUEST_URI']);

        $fileInfo = explode('/', rtrim($uri, '/'));
        

	    if ( $uri == $config->self ) {
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

	    if(file_exists($_SERVER['DOCUMENT_ROOT'].$uri) and ($_SERVER['SCRIPT_FILENAME']!=$_SERVER['DOCUMENT_ROOT'].$_SERVER['REQUEST_URI']) and ($_SERVER['REQUEST_URI']!="/") and !preg_match($this->forbiddenExtensionsRegex, $_SERVER['REQUEST_URI']) and !is_dir($_SERVER['DOCUMENT_ROOT'].$uri)){
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
                
                include( $_SERVER['DOCUMENT_ROOT'].$uri );
                exit();
            }
        }

        /* Check to see if we have a widget... */
            
        if ( preg_match('/\/[A-Za-z0-9]+(\/)*$/', $uri ) && !preg_match('/permalink\/[A-Za-z]+\/[0-9]+(\/)*$/', $uri)) {
	  if ($config->defaultWidget != '') {
	    $oldEH = set_error_handler('defaultWidget');
	    $oldLev = error_reporting(0);
	    global $widgetName;
	    $widgetName = $fileInfo[count($fileInfo)-1];
	    D::log('Attempting to include: '.$widgetName);
	    include_once($widgetName.".inc");
	    restore_error_handler();
	    error_reporting($oldLev);
	    $widget = new $widgetName;
            $widget->display();
            exit;
	  } else {
	    $widget = new $fileInfo[count($fileInfo)-1];
            $widget->display();
	    exit;
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

function defaultWidget($n, $s, $f, $l, $c) {
  if ($n > E_WARNING) {
    print "anApp PHP Error: ".$s." in $f ($l)<br />";
    return FALSE;
  }
  global $config, $widgetName;
  $widget = new $config->defaultWidget($widgetName);
  $widget->display();
  exit;
}

?>