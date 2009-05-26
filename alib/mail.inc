<?PHP
/**
 * aLib mail sending class. Sends a multipart mime message.
 * Note: Anywhere that you may use a string e-mail address, you may
 * also pass a user object.
 *
 * You must set $mail->from and $mail->to. To set the body of the
 * message, set $mail->body. Both the body and the headers are
 * wordwrapped at 70 characters. Eat me.
 *
 * If you set $mail->someHeader = "foo"; and then $mail->someHeader =
 * "bar"; you won't get what you expect. You'll see in the e-mail
 * "SomeHeader: foo, bar". Use "unset ($mail->someHeader);" to reset a header.
 *
 * Also, all header names have their first letter uppercased. So while
 * $mail->to and $mail->To will both produce a "To: " header, since
 * variable names in PHP are case sensitive only one will be reflected
 * in the e-mail message.
 *
 * If you do $var = $mail->message you will get a string
 * representation of your message as it currently stands. Useful for
 * printing.
 * @package aLib
 * @subpackage Mail
 * @version 0.1
 */
class mail extends anObject
  {
    private $_parts = array();
    private $_headers = array();
    private $_body;
    /**
     * Constructor.
     * @param string $from [optional] email address from
     * @param string $to [optional] email address to
     * @param string $subject [optional] subject for e-mail
     */
    
    function __construct($from = NULL, $to = NULL, $subject = NULL) 
      {
        if ($from != NULL) 
          {
            $this->__set('from', $from);
          }
        if ($to != NULL) 
          {
            $this->__set('to', $to);
          }
        if ($subject != NULL) 
          {
            $this->__set('subject', $subject);
          }
        parent::__construct();
      }
    
    function __set($n, $v) 
      {
        if (is_object($v) && (is_subclass_of($v, 'user') || (get_class($v) == 'user'))) 
          {
            $string = $v->gn . " " . $v->sn . " <" . $v->email . ">";
            $v = $string;
          }
        global $debug;
        if ($n == "from" || $n == "subject" || $n == "body") 
          {
            if ($n == "body") 
              {
                $this->_body = $v;
              }
            else
              {
                $this->_headers[$n] = $v;
              }
            $debug->line("Setting " . $n . " to " . $v, 10);
          }
        else
          {
            $this->_headers[$n][] = $v;
            $debug->line("Adding " . $v . " to " . $n, 10);
          }
      }
    
    function __get($n) 
      {
        if ($n == "body") 
          {
            return $this->_body;
          }
        elseif($n == "message") 
          {
            return $this->_generateMessage();
          }
        elseif(is_array($this->_headers[$n])) 
          {
            return implode(',', $this->_headers[$n]);
          }
        else
          {
            return $this->_headers[$n];
          }
      }
    protected 
    function _generateMessage() 
      {
        $body = $this->_generateBody();
        $headers = $this->_generateHeaders();
        $message = $headers . "\n\n" . $body;
        return $message;
      }
    /**
     * sendMessage -- Your message is complete and ready to e-mail.
     * @return bool FALSE means mail was not sent, TRUE means it was.
     */
    
    function sendMessage() 
      {
        $status = FALSE;
        if (sizeOf($this->from) && sizeOf($this->to)) 
          { # must have a from address...
            $message = $this->_generateBody();
            $headers = $this->_generateHeaders();
            
            $status = mail($this->to, $this->subject, $message, $headers);
          }
        if ( $status ) {
            return $status;            
        } else {
            throw new Exception('Could not send mail:\n'.$headers.'\n'.$message);
        }
      }
    /**
     * addAttachment -- add a file attachment to the message.
     * Reads a file from the local filesystem and adds it to the message.
     * @param string $filename Filename to load.
     * @param string $name Name to call the file in the e-mail message. If blank, $filename (minus path) will be used.
     * @param string $ctype Content type. Default is "application/octet-stream" but if the file is text, try "text/plain"
     */
    
    function addAttachment($filename, $name = "", $ctype = "application/octet-stream") 
      {
        $attachment = fread(fopen($filename, "r") , filesize($filename));
        if ($name == "") 
          {
            $name = basename($filename);
          }
        $this->_parts[] = array(
            "ctype" => $ctype,
            "attachment" => $attachment,
            "name" => $name
        );
      }
    protected 
    function _buildMultipart($part) 
      {
        $attachment = $part["attachment"];
        if ($part['ctype'] != "text/plain" && $part['ctype'] != "text/html") 
          {
            $attachment = base64_encode($attachment);
            $encoding = "base64";
          }
        else
          {
            $encoding = $part['ctype'];
          }
        $string = "Content-Type: " . $part["ctype"];
        $string.= ($part["name"]) ? "; name=\"" . $part["name"] . "\"" : "";
        $string.= "\nContent-Transfer-Encoding: $encoding\n\n$attachment\n";
        return $string;
      }
    protected 
    function _generateBody() 
      {
        if (sizeOf($this->_parts)) 
          {
            // Mime message.
            $this->MIME_Version = "1.0";
            $boundary = "a" . md5(uniqid(time()));
            $this->Content_Type = "multipart/mixed; boundary = $boundary";
            $text = "This is a MIME encoded message.\n\n--$boundary\n";
            // First fake up our body as a text attachment:
            $body = array(
                "ctype" => "text/plain",
                "attachment" => $this->body,
                "name" => ""
            );
            $text.= $this->_buildMultipart($body);
            $text.= "--$boundary\n";
            // Now we process attachments.
            foreach($this->_parts as $part) 
              {
                $text.= $this->_buildMultipart($part);
                $text.= "--$boundary\n";
              }
            $text = wordwrap($text, 70);
            return $text;
          }
        else
          {
            // Plain text message.
            $text = $this->_body;
            $text = wordwrap($text, 70);
            return $text;
          }
      }
    protected 
    function _generateHeaders() 
      {
        $headers = "";
        foreach($this->_headers as $header => $content) 
          {
            if ($header != "_body" && $header != '_parts' && $header != "to" && $header != "subject" && $header != "_header" && $header != "body") 
              {
                $headerText = preg_replace('/_/', '-', $header);
                //$headers .= wordwrap(ucfirst($headerText) . ": " . $this->$header . "\n", 70);
                $headers.= ucfirst($headerText) . ": " . $this->$header . "\n";
              }
          }
        return $headers;
      }
  }
?>
