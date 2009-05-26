<?PHP
/**
 * aLib time functions. Time is a very slippery thing, what with
 * timezones, inconveniently inconsistent month lengths, etc. These
 * are some helpful ways of dealing with time. The driving philosophy
 * behind this is to always think of time in seconds. See also {@link
 * time} for information on how to use seconds.
 *
 * @package aLib
 * @subpackage Time
 */
// A bit of a help:

/**
 * @constant a date format string that returns an offical UTC format time string.
 * Just for your info, that looks like 20050427T230705Z. I hate UTC Dates.
 */
define("UTCFORMAT", "Ymd\THis\Z"); // Turns a date or gmdate ot pmTZDate call into UTC formatted string.
global $AGLOBAL;
/**
 * @global mixed $AGLOBAL['timeModifiers'] List of timezones and their modifiers.
 */
$AGLOBAL['timeModifiers'] = array(
    "GMT" => 0,
    "EST5EDT" => -5,
    "CST6CDT" => -6,
    "MST7MDT" => -7,
    "MST" => -7,
    "PST8PDT" => -8
);
/*
* Day Array
*
* Useful if you can't be bothered to look up whether Sunday is day 0 or 7
* @global mixed $AGLOBAL['daysOfTheWeek'] days of the week keyed to their number.
*/
$AGLOBAL['daysOfTheWeek'] = array(
    "sunday" => 0,
    "monday" => 1,
    "tuesday" => 2,
    "wednesday" => 3,
    "thursday" => 4,
    "friday" => 5,
    "saturday" => 6
);
/**
 * @constant number of seconds in 30 minutes
 */
define("ANHALFHOUR", 1800); // Half Hour

/**
 * @constant number of seconds in an hour
 */
define("ANHOUR", 3600); // Hour

/**
 * @constant number of seconds in one day
 */
define("ADAY", 86400);
/**
 * @constant number of seconds in one week
 */
define("AWEEK", 604800);
/**
 * @constant number of seconds in a 365 day year
 */
define("AYEAR", 31557600);
/**
 * @constant number of seconds in a 366 day year
 */
define("ALEAPYEAR", 31622400);
/**
 * Returns the number of seconds in an half hour. Use this instead of
 * the constants so you get in the habit when it comes to "aMonth."
 * @return int 1800
 */

function anHalfHour($time) 
  {
    return ANHALFHOUR;
  }
/**
 * Returns the number of seconds in an hour. Use this instead of
 * the constants so you get in the habit when it comes to "aMonth."
 * @return int 3600
 */

function anHour($time) 
  {
    return ANHOUR;
  }
/**
 * Returns the number of seconds in a day. Use this instead of
 * the constants so you get in the habit when it comes to "aMonth."
 * @return int 86400
 */

function aDay($time) 
  {
    return ADAY;
  }
/**
 * Returns the number of seconds in a week. Use this instead of
 * the constants so you get in the habit when it comes to "aMonth."
 * @return int 604800
 */

function aWeek($time) 
  {
    return AWEEK;
  }
/**
 * Returns the number of seconds in a given month. If no argument is
 * given, the current month is assumed.
 * @param int $time A utime somewhere in the month you want to know how many seconds are in.
 * @param int $timezone Shouldn't matter, months are the same no matter the timezone, but just in case, you can specify one.
 * @return int number of seconds
 */

function aMonth($time = NULL, $timezone = "EST5EDT") 
  {
    if (!$time) 
      {
        $time = time();
      }
    $mnum = date('m', $time);
    $ynum = date('Y', $time);
    $times = monthStartEnd($mnum, $ynum, $timezone);
    $diff = $times[1]-$times[0];
    return $diff;
  }
/** 
 * Returns the number of seconds in a given year. If no argument is
 * given, the current year is assumed. Leap years are taken into
 * account.
 * @param int $time A utime somewhere in the month you want to know how many seconds are in.
 * @return int number of seconds in a year
 */

function aYear($time = NULL) 
  {
    if (!$time) 
      {
        $time = time();
      }
    if (date('L', $time)) 
      {
        return ALEAPYEAR;
      }
    else
      {
        return AYEAR;
      }
  }
/**
 * Seconds in a Human Readable string
 *
 * Returns a string representation of the hours, days, weeks, etc. in a number of seconds.
 *
 * secondsHumanReadable(86465); would return:
 * "1 day, 1 minute and 5 seconds"
 *
 * Note, because of the mutability of number of seconds in a month, we
 * stop making things readable at weeks. So you may end up with:
 * "87 weeks, 2 days and 3 minutes"
 *
 * @param int $secondsIn seconds to convert
 * @return string
 * @author BrYan P. Johnson
 */

function secondsHumanReadable($secondsIn) 
  {
    $weeks = floor($secondsIn/604800); // 7 days a week.
    $secondsIn = $secondsIn-($weeks*604800); // Remove the weeks.
    $days = floor($secondsIn/86400); // 24 hours a day.
    $secondsIn = $secondsIn-($days*86400); // Remove the days.
    $hours = floor($secondsIn/3600); // 60 minutes in anhour
    $secondsIn = $secondsIn-($hours*3600); // Lather.
    $minutes = floor($secondsIn/60); // Rinse.
    $seconds = $secondsIn-($minutes*60); // Repeat.
    //Holy Trinary Operators, Batman!
    $retval.= ($weeks > 0) ? $weeks . (($weeks > 1) ? " weeks " : " week "):
        "";
        $retval.= ($days > 0) ? $days . (($days > 1) ? " days " : " day "):
            "";
            $retval.= ($hours > 0) ? $hours . (($hours > 1) ? " hours " : " hour "):
                "";
                $retval.= ($minutes > 0) ? $minutes . (($minutes > 1) ? " minutes " : " minute "):
                    "";
                    $retval.= ($seconds > 0) ? $seconds . (($seconds > 1) ? " seconds " : " second "):
                        "";
                        $retval = preg_replace("/ (\d+) /", ", $1 ", $retval);
                        $retval = preg_replace("/ $/", "", $retval);
                        return $retval;
                      }
                    /**
                     * TZDate
                     *
                     * Returns a formatted string of the date of a given utime.
                     *  formatted string is just like for the builtin date and gmdate functions.
                     *
                     * @param string $timezone String representation of the timezone (As in $AGLOBAL['timeModifiers'])
                     * @param string $formatstring Format string
                     * @param int $time the utime to format. If left blank, the current time is assumed.
                     * @return string
                     * @author BrYan P. Johnson
                     */
                    
                    function TZDate($timezone = "EST5EDT", $formatstring = UTCFORMAT, $time = NULL) 
                      {
                        global $AGLOBAL, $debug;
                        if (!isset($time)) 
                          {
                            $time = time();
                          }
                        $modifier = $AGLOBAL['timeModifiers'][$timezone];
                        if (isDST($timezone, $time)) 
                          {
                            $modifier++;
                          }
                        $gettime = $time+(3600*$modifier);
                        $getformat = preg_replace("/T/", "@@@", $formatstring);
                        $retstring = gmdate($getformat, $gettime);
                        $retval = preg_replace("/@@@/", $timezone, $retstring);
                        return $retval;
                      }
                    /**
                     * Daylight Savings Time
                     *
                     * Checks to see if it's DST, and if it is, if the given time zone
                     * is one which observes it.
                     *
                     * @param string timezone
                     * @param int timestamp
                     * @return bool 0 if no, 1 if yes
                     * @author Norbert Taylor
                     */
                    
                    function isDST($timezone = NULL, $timestamp = NULL) 
                      {
                        $isDST = 0;
                        if (empty($timestamp)) 
                          {
                            $timestamp = time();
                          }
                        if (empty($timezone)) 
                          {
                            $timezone = 'EST5EDT';
                          }
                        $timeInfo = localtime($timestamp, 1);
                        if ($timeInfo['tm_isdst'] && preg_match("/DT/", $timezone)) 
                          {
                            $pmDST = 1;
                          }
                        return $pmDST;
                      }
                    /**
                     * Parse Time Zone
                     *
                     * Returns the string for the current timezone (i.e. EDT, EST, CST, MST,
                     * PDT, etc)
                     *
                     * @param string timezone
                     * @param int timestamp
                     * @return    string
                     * @author Norbert Taylor
                     */
                    
                    function parseTZ($timezone = NULL, $timestamp = NULL) 
                      {
                        if (empty($timezone)) 
                          {
                            $timezone = 'EST5EDT';
                          }
                        preg_match('/([a-z]+)[0-9]?([a-z]+DT)?/i', $timezone, $match);
                        if (isDST($timezone, $timestamp)) 
                          {
                            $string = $match[2];
                          }
                        else
                          {
                            $string = $match[1];
                          }
                        return $string;
                      }
                    /**
                     * TZ Maketime
                     *
                     * Converts a date into utime, modified by the timezone.
                     *
                     * @param string $timezone String representation of the timezone (As in $pmTimeModifiers)
                     * @param int $hour
                     * @param int $minute
                     * @param int $second
                     * @param int $month
                     * @param int $day
                     * @param int $year
                     * @param int $is_dst
                     * @return int
                     * @author Norbert Taylor
                     */
                    
                    function TZmktime($timezone, $hour, $minute, $second, $month, $day, $year, $is_dst = NULL) 
                      {
                        global $AGLOBAL;
                        $modifier = $AGLOBAL['timeModifiers'][$timezone];
                        $hour-= $modifier;
                        $stamp = gmmktime($hour, $minute, $second, $month, $day, $year);
                        #need to make sure the new stamp is adjusted for DST.
                        if ($is_dst || isDST($timezone, $stamp)) 
                          {
                            $stamp-= 3600;
                          }
                        return $stamp;
                      }
                    /**
                     * Get Mondays
                     *
                     * Returns a set of timestamps for Mondays (at 12 midnight).
                     * The call
                     *     getMondays()
                     * would return the timestamp for the most recent Monday, at 12 midnight ET
                     *
                     * The call
                     * 	  getMondays(-2, 2, "PST8PDT")
                     * would return the timestamps for two Mondays -- the one before the most recent,
                     * and the one before that -- @ midnight Pacific time.
                     *
                     * use TZDate() for printing
                     *
                     * @param int $start week to start at; by default it's 0, the most recent Monday
                     * @param int $number number of weeks to retreive
                     * @param string $timezone the timezone these timestamps calculated in
                     * @param int $timestamp the timestamp you're checking.
                     * @return array
                     * @author Norbert Taylor
                     */
                    
                    function getMondays($start = NULL, $number = NULL, $timezone = NULL, $timestamp = NULL) 
                      {
                        return getDays('Monday', $start, $number, $timezone, $timestamp);
                      }
                    /**
                     * getDays returns utimes for midnight for x number of certain days of the week.
                     *
                     * For example: <code>getDays('Tuesday', 0, 4, 'EST5EDT');</code>
                     * would return an array of four timestamps corresponding to midnight
                     * on each of the next 4 Tuesdays.
                     *
                     * @param string $wantedday String representation of the day of the week you want.
                     * @param int $start How many weeks in the future (positive integers) or past (negative integers) you want to start counting from.
                     * @param int $number How many timestamps to return
                     * @param string $timezone The timezone you wish to use.
                     * @param int $timestamp The timestamp at which to figure all of this from (defaults to now.)
                     * @return mixed An array of timestamps.
                     * @author Norbert Taylor [ed: I assume. --Bryan]
                     */
                    
                    function getDays($wantedday, $start = 0, $number = 1, $timezone = "EST5EDT", $timestamp = NULL) 
                      {
                        global $AGLOBAL;
                        if (empty($timestamp)) 
                          {
                            $timestamp = time();
                          }
                        $dateinfo = getdate($timestamp);
                        $weekday = $dateinfo['wday'];
                        $month = $dateinfo['mon'];
                        $day = $dateinfo['mday'];
                        $year = $dateinfo['year'];
                        $modifier = $AGLOBAL['timeModifiers'][$timezone];
                        $myday = $AGLOBAL['daysOfTheWeek'][strtolower($wantedday) ];
                        if (!($weekday == $myday)) 
                          {
                            $dayspread = $myday-$weekday;
                            if ($dayspread > 0) 
                              {
                                $diff = aWeek() -(aDay() *$dayspread);
                              }
                            else
                              {
                                $diff = (-1*(aDay() *$dayspread));
                              }
                          }
                        else
                          {
                            $diff = 0;
                          }
                        $oldtime = TZmktime($timezone, 0, 0, 0, $month, $day, $year);
                        $oldtime-= $diff;
                        $dstMod = isDST($timezone, $oldtime);
                        $week = aWeek();
                        if ($start == 0) 
                          {
                            $highNum = ($number-$start);
                          }
                        else
                          {
                            $highNum = ($number+$start);
                          }
                        for ($i = $start, $counter = 0;$i < $highNum;$i++, $counter++) 
                          {
                            $timestamps[$counter] = $oldtime+($week*($i));
                            if (isDST($timezone, $timestamps[$counter]) > $dstMod) 
                              {
                                $timestamps[$counter]-= 3600;
                              }
                            elseif(isDST($timezone, $timestamps[$counter]) < $dstMod) 
                              {
                                $timestamps[$counter]+= 3600;
                              }
                          }
                        return $timestamps;
                      }
                    /**
                     * Week Of
                     *
                     * Returns the timestamp for the Monday (at 12 midnight) before a certain time
                     * (if no time is given, it uses current time).
                     *
                     * To print the time, use TZDate();
                     *
                     * @param string $timezone the timezone these timestamps calculated in
                     * @param int $timestamp the timestamp you're checking.
                     * @return int
                     * @author Norbert Taylor
                     */
                    
                    function weekOf($timezone = "EST5EDT", $timestamp = NULL) 
                      {
                        if (empty($timestamp)) 
                          {
                            $timestamp = time();
                          }
                        $monday = getMondays("", "", $timezone, $timestamp);
                        return $monday[0];
                      }
                    /**
                     * Month Start and End
                     *
                     * Returns utime for the first and last seconds of a month
                     *
                     * To print the time, use TZDate();
                     *
                     * @param int $month
                     * @param int $year
                     * @return mixed array where [0] is the start value; [1] is the end value
                     * @author Norbert Taylor <ntaylor@corp.earthlink.net>
                     */
                    
                    function monthStartEnd($month = "", $year = "", $timezone = "EST5EDT") 
                      {
                        global $AGLOBAL;
                        $times = array();
                        if (empty($month)) 
                          {
                            $info = getdate();
                            $month = $info['mon'];
                            $year = ($year) ? $year : $info['year'];
                          }
                        if ($month == 12) 
                          {
                            $nextMonth = 1;
                            $nextYear = $year+1;
                          }
                        else
                          {
                            $nextMonth = $month+1;
                            $nextYear = $year;
                          }
                        $startTime = gmmktime(0, 0, 0, $month, 1, $year);
                        $endTime = gmmktime(0, 0, 0, $nextMonth, 1, $nextYear);
                        $startModifier = $endModifier = $AGLOBAL['timeModifiers'][$timezone];
                        if (isDST($timezone, $startTime)) 
                          {
                            $startModifier++;
                          }
                        if (isDST($timezone, $endTime)) 
                          {
                            $endModifier++;
                          }
                        $startAdjuster = ($startModifier*3600);
                        $endAdjuster = ($endModifier*3600);
                        $times[0] = $startTime-= $startAdjuster;
                        $times[1] = ($endTime-= $endAdjuster) -1;
                        return $times;
                      }
                    /**
                     * Get Next Month
                     *
                     * Given a Unix timestamp, return the timestamp
                     * for the first day of a subsequent month.
                     *
                     * @param int $timestamp the timestamp you're checking.
                     * @param string $timezone the timezone these timestamps calculated in
                     * @param int $cycle how many months in the future do you want the timestamp for?
                     * @return int timestamp
                     * @todo update this documentation.
                     * @author John Clark
                     */
                    
                    function getNextMonth($timestamp, $timezone = "EST5EDT", $cycle = 1) 
                      {
                        $month = TZdate("m", $timestamp, $timezone);
                        $month = (int)$month;
                        $year = TZdate("Y", $timestamp, $timezone);
                        $year = (int)$year;
                        $month = $month+$cycle;
                        $nextMonth = $month%12;
                        $nextYear = $year;
                        if ($month > 12) 
                          {
                            $nextYear = $year+1+($cycle%12); #If the cycle is > 12, we go up one more year.
                            
                          }
                        $nextmonth = TZmktime($timezone, 0, 0, 0, $nextMonth, 1, $nextYear);
                        return $nextmonth;
                      }
                    /**
                     * currentBillingMonth
                     *
                     * Given a Unix timestamp, return the timestamp
                     * for the first day of the current month.
                     * mainly used to find values based on timestamps
                     * stored in the db as opposed to monthStartEnd
                     * which is looking for a month and date value
                     * saves having to convert a timestamp and then pass the values
                     *
                     * @param int $timestamp the timestamp you're checking.
                     * @param string $timezone the timezone these timestamps calculated in
                     * @return int timestamp
                     * @author John Clark [ed- I presume. Bryan]
                     */
                    
                    function currentBillingMonth($timestamp, $timezone = "EST5EDT") 
                      {
                        if (!$timestamp) 
                          {
                            $timestamp = time();
                          }
                        $month = intval(TZDate($timezone, "n", $Timestamp));
                        $year = intval(TZDate($timezone, "Y", $myTimestamp));
                        return mktime(0, 0, 0, $month, 1, $year);
                      }
                    /**
                     * Convenience function. Can never remember date format strings.
                     *
                     * @param int $timestamp the timestamp you're checking.
                     * @param string $timezone the timezone these timestamps calculated in
                     * @return int Numeric month of the timestamp.
                     * @author John Clark [ed- I presume. Bryan]
                     */
                    
                    function monthOf($timestamp, $timezone = "EST5EDT") 
                      {
                        return intval(TZDate($timezone, "n", $myTimestamp));
                      }
                    /**
                     * Convenience function. Can never remember date format strings.
                     *
                     * @param int $timestamp the timestamp you're checking.
                     * @param string $timezone the timezone these timestamps calculated in
                     * @return int numeric year of the timestamp
                     * @author John Clark [ed- I presume. Bryan]
                     */
                    
                    function yearOf($timestamp, $timezone = "EST5EDT") 
                      {
                        return intval(TZDate($timezone, "Y", $myTimestamp));
                      }
                    /**
                     * Convenience function. Can never remember date format strings.
                     *
                     * @param int $timestamp the timestamp you're checking.
                     * @param string $timezone the timezone these timestamps calculated in
                     * @return int numeric day of the month for the timestamp.
                     * @author John Clark [ed- I presume. Bryan]
                     */
                    
                    function dayOf($timestamp, $timezone = "EST5EDT") 
                      {
                        return intval(TZDate($timezone, "j", $myTimestamp));
                      }
                    /**
                     * Given a time string, return a timestamp.
                     *
                     * @param string $aDate String in the "mm/dd/yyyy" format.
                     * @param string $aTime [optional] String in the "hh:mm:ss" format.
                     * @param string $timezone the timezone these timestamps calculated in
                     * @return int timestamp or 0 for bad format.
                     * @author John Clark
                     * @todo Check documentation.
                     */
                    
                    function parseTimeString($aDate, $aTime = NULL, $timezone = "EST5EDT") 
                      {
                        global $AGLOBAL, $debug;
                        // First we check the format
                        if (ereg("([0-9]{1,2})\/([0-9]{1,2})\/([0-9]{4})", $aDate, $regs)) 
                          {
                            $m = $regs[1];
                            $d = $regs[2];
                            $y = $regs[3];
                          }
                        else
                          {
                            $debug->line("Invalid Date Format: $aDate. Date should be in x/x/xxxx format", 7);
                            return 0;
                          }
                        // check to see if we were passed a time value
                        if ($aTime != "") 
                          {
                            // we got a time value so parse it
                            if (ereg("([0-9]{1,2}):([0-9]{1,2}) *(A|a|P|p)(M|m)", $aTime, $regs)) 
                              {
                                if ($regs[3] == "p" || $regs[3] == "P") 
                                  {
                                    $h = $regs[1]+12;
                                  }
                                else
                                  {
                                    $h = $regs[1];
                                  }
                                $mn = $regs[2];
                                $good_time = true;
                                // print "regs : 1:".$regs[1]." 2:".$regs[2]." 3:".$regs[3]." -- h and mn : $h, $mn";
                                
                              }
                            else
                              {
                                $debug->line("Invalid Time Format: $aTime. Time should be in x:xx am format<br>", 7);
                                return 0;
                              }
                          }
                        else
                          {
                            // we got no time values so just set to default of 1am
                            // this should only occur when the user has no interest in the time value
                            $h = 1;
                            $mn = 0;
                          }
                        $debug->line("Doing TZmktime for $m/$d/$y $h:$mn $timezone", 7);
                        // now if we know that we have a valid date and time we make a timestamp and return it
                        return TZmktime($timezone, $h, $mn, 0, $m, $d, $y);
                      }
                    /**
                     * Returns the utime for a day a certain number of business
                     * days in the future. Has no clue about holidays.
                     *
                     * @param int $days Number of business days to count ahead.
                     * @param int $utime [optional] Optional start time.
                     * @return int timestamp
                     * @author BrYan P. Johnson
                     */
                    
                    function addBusinessDays($days, $utime = NULL) 
                      {
                        if (!$utime) 
                          {
                            $utime = time();
                          }
                        for ($i = 1;$i <= $days;$i++) 
                          {
                            $utime = $utime+ADAY;
                            $dow = date("w", $utime);
                            if ($dow == 0 || $dow == 6) 
                              {
                                $days++;
                              }
                          }
                        return $utime;
                      }
?>