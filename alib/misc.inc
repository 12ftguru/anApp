<?PHP
/**
 * Miscellaneous functions.
 * Since this isn't an object, you have to include it with include_once('misc.inc');
 * The goal is to move functions from this file to the right place when that becomes apparent.
 * @package aLib
 * @subpackage Utility
 */
/**
 * Returns an array of numbers given a description of that array.
 *
 * Takes a comma separated list (or comma and space) as follows:
 * 1, 2, 4-7, 10
 * It would then return an array ('1', '2', '4', '5', '6', '7', '10');
 * @param string $description Description of the list of numbers.
 * @param bool $sort Sort the numbers.
 * @return array $numbers The array of numbers.
 */

function arrayFromDescription($description, $sort = TRUE) 
  {
    $numbers = array();
    $temp = preg_split("/,\s*/", $description);
    foreach($temp as $number) 
      {
        if (preg_match('/(\d+)-(\d+)/', $number, $matches)) 
          {
            $start = $matches[1];
            $end = $matches[2];
            for ($i = $start;$i++;$i == $end) 
              {
                $numbers[] = $i;
              }
          }
        else
          {
            $numbers[] = $number;
          }
      }
    if ($sort) 
      {
        sort($numbers);
      }
    reset($numbers);
    return $numbers;
  }
/**
 * Returns the where portion of an sql statement given a description and a column name.
 * Takes a comma separated list (or comma and space) as follows:
 * 1, 2, 4-7, 10
 * Given the other arguments (columnname = "foo", bool = "OR") It would return:
 * foo = '1' OR foo = '2' OR (foo <= '7' AND foo >= '4') OR foo = '10'
 *
 * @param string $columnname Name of the column
 * @param string $description Description of the list of numbers.
 * @param string $bool What kind of boolean to separate our stuff with.
 * @return string $where The clause.
 */

function sqlWhereFromDescription($columnname, $description, $bool = "OR") 
  {
    global $debug;
    $temp = array();
    $count = 0;
    $temp = preg_split("/,\s*/", $description);
    $where = "";
    $debug->line('temp', 4);
    $debug->variable($temp, 4);
    foreach($temp as $tempString) 
      {
        if (preg_match('/(\d+)-(\d+)/', $tempString, $matches)) 
          {
            $start = $matches[1];
            $end = $matches[2];
            $tempString = "($columnname <= '$end' AND $columnname >= '$start')";
          }
        else
          {
            $tempString = "$columnname = '$tempString'";
          }
        $where.= ($count > 0) ? " OR " . $tempString : $tempString;
        $count = 1;
      }
    return $where;
  }
//Strips $ and ,. If there is a (), removes it and adds a -. Then casts as an float.

function dollarToDecimal($string) 
  {
    $string = str_replace(',', '', $string);
    $string = str_replace('$', '', $string);
    if (!(strpos($string, '(') === false)) 
      {
        $string = str_replace('(', '', $string);
        $string = str_replace(')', '', $string);
        $string = "-" . $string;
      }
    return floatval($string);
  }
?>
