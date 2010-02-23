<?php
//error_reporting(E_ALL);
//ini_set('display_errors',1);
ini_set('always_populate_raw_post_data', 1);

$configFile = 'common/config.inc';
include_once("./alib/alib.inc");
global $debug, $config;

addIncludePath('./alib');
addIncludePath('./common');
addIncludePath('./php', TRUE);
include_once( '../alib/iuser.inc' );
include_once( './common/functions.inc' );
include_once( './common/login.inc' );
include_once( './common/smartObjectDefs.inc' );

// Connect to the db:
if ( ! is_object($db) ) {
  $db = new idb($config->mainDB);                
}
D::log('db');
D::v($db);


$login = new $config->loginModule;




if ( $login->loggedIn || $config->allowNonLoggedIn ) {
    global $user, $broker;
$broker = new broker(); 
} elseif (stristr($_SERVER['REQUEST_URI'], 'api')) {
  $api = new publicAPI();
} else {
    $template = new template($config->loginTemplate);
    $template->set('title', $config->defaultTitle);
    $template->set('appName', $config->appName);
    $template->set('extLocation', $config->extLocation);
    $template->set('self', $config->self);
    if ( $login->error && $login->error != 'Not logged in and not trying to log in.') {
        $template->set('badLogin', TRUE);
	$template->set('loginError', $login->error);
    }
    $template->display();
}
?>