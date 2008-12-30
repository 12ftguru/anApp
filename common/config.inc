<?php

include_once('dbinfo.inc');

/* What level is an admin user? */
define( ANAPPADMIN, 1 );

/* Are we in production? */
$deployed = FALSE;

/* Where are we? */
$appURL = 'http://127.0.0.1/anApp2';

/* Global Templates */
$loginTemplate = "login.ihtml";
$loggedInTemplate = "index.ihtml";

/* User Login Expiration Time */
$loginExpires = 60*60*8; #8 hours

/* Default long page title */
$defaultTitle = '';

/* Application name (used for cookies) */
$appName = '';

/* Where does ext live? */
$extLocation = '../ext2';

/* Extra javascript files */
$javascripts = array('../firebug/firebugx.js');

/* Extra css files */
$styles = array();

/* What's our root directory? */
$self = '/anApp2/';

?>
