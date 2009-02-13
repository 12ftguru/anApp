<?php
$debugLevel = 6;
$debugStyle = 'firebug';
$configFile = 'common/config.inc';
include_once("./alib/alib.inc");
global $debug;

$debug->debug('Adding paths...', 10);
addIncludePath('./alib');
addIncludePath('./common');
addIncludePath('./php', TRUE);

$debug->debug('Adding mandatory includes...', 10);
include_once( './alib/iuser.inc' );
include_once( './common/functions.inc' );

$debug->debug('Creating login object.', 10);
$login = new login();



/*
   Here we turn off error reporting if debugging is 0 (we assume we're
   in production and don't want people seeing how we made the sausage.
   If debug level is > 8 we turn error reporting all the way on as well.
*/
//    error_reporting(0);
//error_reporting(E_ALL ^ (E_WARNING|E_NOTICE));


/* if ( $debugLevel == 0 ) { */
/*     error_reporting(0);     */
/* } elseif ( $debugLevel > 8 ) { */
/*     error_reporting(E_ALL); */
/* } */

//if ( $login->loggedIn ) {
    global $user, $broker;
    $user = $login->user;
    $debug->debug('User logged in: %s', $user, 3);
    $broker = new broker(); 
    

    
/*} else {
    $debug->debug('No user logged in: %s', $login, 3);
    $template = new template($config->loginTemplate);
    $template->set('title', $config->defaultTitle);
    $template->set('appName', $config->appName);
    $template->set('extLocation', $config->extLocation);
    $template->set('self', $config->self);
    if ( $login->error == BADLOGIN ) {
        $template->set('badLogin', TRUE);
    }
    $template->display();
    }*/
?>