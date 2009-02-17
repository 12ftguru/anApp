<?php
$debugLevel = 6;
$debugStyle = 'firebug';
$configFile = 'common/config.inc';
include_once("./alib/alib.inc");
global $debug, $config;

$debug->debug('Adding paths...', 10);
addIncludePath('./alib');
addIncludePath('./common');
addIncludePath('./php', TRUE);

$debug->debug('Adding mandatory includes...', 10);
include_once( './alib/iuser.inc' );
include_once( './common/functions.inc' );
include_once( './common/login.inc' );

// Connect to the db:
if ( ! is_object($db) ) {
  $db = new idb($config->mainDB);                
}

$debug->debug('Creating login object '.$config->loginModule, 10);
$login = new $config->loginModule;




if ( $login->loggedIn ) {
    global $user, $broker;
    $debug->debug('User logged in: %s', $user->loginName, 3);
    $broker = new broker(); 
    

    
} else {
    $debug->debug('No user logged in: %s', $login, 3);
    $template = new template($config->loginTemplate);
    $template->set('title', $config->defaultTitle);
    $template->set('appName', $config->appName);
    $template->set('extLocation', $config->extLocation);
    $template->set('self', $config->self);
    if ( $login->error ) {
        $template->set('badLogin', TRUE);
	$template->set('loginError', $login->error);
    }
    $template->display();
}
?>