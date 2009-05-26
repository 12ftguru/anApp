<?php
/**
 *An object for handling file uploads...
 * Works like this:
 *<code>$upload = new upload;
 * if ($upload->verify($formfieldname, $acceptable_types)) {
 * $upload->save_file($path);
 * }</code>
 *
 * You can also read a file line by line if it's been verified:
 *
 *<code>while ($line = $upload->read_file()) {
 * #do stuff
 * }</code>
 *
 * Acceptable types is a pipe delimited list of mime-types:
 * <code>$acceptable_types = "image/gif";  #only gif images
 * $acceptable_types = "image/gif|image/png|image/jpg"; #gif, jpg or img
 * $acceptable_types = "";  #all types accepted.</code>
 *
 * If the file is an image, you can specify a max height and width in pixels:
 * $upload->max_image_size('400', '600');
 *
 * You can also specify a max file size for any file in bytes:
 * $upload->max_filesize(10000);  #limit to 10k files.
 * @version 0.1
 * @package aLib
 * @subpackage Utility
 */
class upload extends anObject
  {
    var $file;
    var $errors;
    var $accepted;
    var $max_filesize;
    var $max_image_width;
    var $max_image_height;
    var $disallowed_files_regex = '/\.(htm|html|php|js|txt)$/';
    protected $_fh;
    /**
     * Sets the max filesize to $size
     * @param int $size
     */
    
    function max_filesize($size) 
      {
        $this->max_filesize = $size;
      }
    /**
     * Sets the maximum image size to $width by $height
     * @param int $width
     * @param int $height
     */
    
    function max_image_size($width, $height) 
      {
        $this->max_image_width = $width;
        $this->max_image_height = $height;
      }
    /**
     * Checks if the file is acceptable and copies it to
     * @param string $filefield form field name of uploaded file
     * @param $accept_type acceptable mime-types, a pipe delimited list of mime-types
     * @param $extension default filename extension
     * @return bool TRUE for file passing tests, FALSE for failure. Check $upload->errors for error details.
     */
    
    function verify($filefield = '', $accept_type = '', $extension = '') 
      {
        if (!is_array($_FILES[$filefield]) || !$_FILES[$filefield]['name']) 
          {
            pre($_FILES);
            $this->errors[0] = "No file was uploaded.";
            $this->accepted = FALSE;
            return FALSE;
          }
        $this->file = $_FILES[$filefield];
        $this->file['file'] = $filefield;
        if (!isset($this->file['extension'])) $this->file['extension'] = "";
        if (!isset($this->file['type'])) $this->file['type'] = "";
        if (!isset($this->file['size'])) $this->file['size'] = "";
        if (!isset($this->file['width'])) $this->file['width'] = "";
        if (!isset($this->file['height'])) $this->file['height'] = "";
        if (!isset($this->file['tmp_name'])) $this->file['tmp_name'] = "";
        if (!isset($this->file['raw_name'])) $this->file['raw_name'] = "";
        if ($this->max_filesize && ($this->file["size"] > $this->max_filesize)) 
          {
            $this->errors[1] = "Maximum file size exceeded. File may be no larger than " . $this->max_filesize/1000 . "KB (" . $this->max_filesize . " bytes).";
            $this->accepted = FALSE;
            return FALSE;
          }
        if (stristr($this->file["type"], "image")) 
          {
            /* IMAGES */
            $image = getimagesize($this->file["tmp_name"]);
            $this->file["width"] = $image[0];
            $this->file["height"] = $image[1];
            # test max image size
            if (($this->max_image_width || $this->max_image_height) && (($this->file["width"] > $this->max_image_width) || ($this->file["height"] > $this->max_image_height))) 
              {
                $this->errors[2] = "Maximum image size exceeded. Image may be no more than " . $this->max_image_width . " x " . $this->max_image_height . " pixels";
                $this->accepted = FALSE;
                return FALSE;
              }
            # Image Type is returned from getimagesize() function
            switch ($image[2]) 
              {
            case 1:
                $this->file["extension"] = ".gif";
            break;
            case 2:
                $this->file["extension"] = ".jpg";
            break;
            case 3:
                $this->file["extension"] = ".png";
            break;
            case 4:
                $this->file["extension"] = ".swf";
            break;
            case 5:
                $this->file["extension"] = ".psd";
            break;
            case 6:
                $this->file["extension"] = ".bmp";
            break;
            case 7:
                $this->file["extension"] = ".tif";
            break;
            case 8:
                $this->file["extension"] = ".tif";
            break;
            default:
                $this->file["extension"] = $extension;
            break;
              }
          }
        elseif(!ereg("(\.)([a-z0-9]{3,5})$", $this->file["name"]) && !$extension) 
          {
            switch ($this->file["type"]) 
              {
            case "text/plain":
                $this->file["extension"] = ".txt";
            break;
            case "text/richtext":
                $this->file["extension"] = ".txt";
            break;
            default:
            break;
              }
          }
        else
          {
            $this->file["extension"] = $extension;
          }
	  # Check to see if we're uploading an allowed file...
	  if (preg_match($this->disallowed_files_regex, $this->file["extension"]) || preg_match($this->disallowed_files_regex, $this->file["name"])) {
	    $this->accepted = FALSE;
	    $this->errors[5] = "You are attempting to upload a disallowed file.";
	    return FALSE;
	  }
	  
	  if ($accept_type && $this->accepted !== FALSE) 
          {
          # check to see if the file is of type specified
            if (stristr($accept_type, $this->file["type"])) 
              {
                $this->accepted = TRUE;
              }
            else
              {
                $this->accepted = FALSE;
                $this->errors[3] = "Only " . str_replace("|", " or ", $accept_type) . " files may be uploaded. Your file is an ".$this->file["type"];
              }
          }
        else
          {
            $this->accepted = TRUE;
          }
        $this->_fh = NULL;
        return $this->accepted;
      }
    /**
     * Cleans up the filename, copies the file from PHP's temp location to $path, and checks the overwrite_mode
     * @param string $path Path to your upload directory
     * @param int $overwrite_mode 1 = overwrite existing file, 2 = create new file with incremented extension, 3 = do nothing but error if file already exists
     */
    
    function save_file($path, $overwrite_mode = "3") 
      {
        if ($path[strlen($path) -1] != "/") 
          {
            $path = $path . "/";
          }
        $this->path = $path;
        $copy = "";
        $n = 1;
        $aok = false;
        if ($this->accepted) 
          {
            # Clean up file name (only lowercase letters, numbers and underscores)
            $this->file["name"] = ereg_replace("[^a-z0-9._]", "", str_replace(" ", "_", str_replace("%20", "_", strtolower($this->file["name"]))));
            # Clean up text file breaks
            if (stristr($this->file["type"], "text")) 
              {
                $this->cleanup_text_file($this->file["tmp_name"]);
              }
            # get the raw name of the file (without it's extension)
            if (ereg("(\.)([a-z0-9]{2,5})$", $this->file["name"])) 
              {
                $pos = strrpos($this->file["name"], ".");
                if (!$this->file["extension"]) 
                  {
                    $this->file["extension"] = substr($this->file["name"], $pos, strlen($this->file["name"]));
                  }
                $this->file['raw_name'] = substr($this->file["name"], 0, $pos);
              }
            else
              {
                $this->file['raw_name'] = $this->file["name"];
                if ($this->file["extension"]) 
                  {
                    $this->file["name"] = $this->file["name"] . $this->file["extension"];
                  }
              }
            switch ($overwrite_mode) 
              {
            case 1: # overwrite mode
                $aok = copy($this->file["tmp_name"], $this->path . $this->file["name"]);
                if ( !$aok ) {
                    $this->errors[4] = "File &quot" . $this->path . $this->file["name"] . "&quot cannot be created.";
                }
            break;
            case 2: # create new with incremental extension
                while (file_exists($this->path . $this->file['raw_name'] . $copy . $this->file["extension"])) 
                  {
                    $copy = "_copy" . $n;
                    $n++;
                  }
                $this->file["name"] = $this->file['raw_name'] . $copy . $this->file["extension"];
                $aok = copy($this->file["tmp_name"], $this->path . $this->file["name"]);
                if ( !$aok ) {
                    $this->errors[4] = "File &quot" . $this->path . $this->file["name"] . "&quot cannot be created.";
                }
            break;
            case 3: # do nothing if exists, highest protection
                if (file_exists($this->path . $this->file["name"])) 
                  {
                    $this->errors[4] = "File &quot" . $this->path . $this->file["name"] . "&quot already exists";
                    $aok = null;
                  }
                else
                  {
                    $aok = copy($this->file["tmp_name"], $this->path . $this->file["name"]);
                    if ( !$aok ) {
                        $this->errors[4] = "File &quot" . $this->path . $this->file["name"] . "&quot cannot be created.";
                    }
                  }
            break;
            default:
            break;
              }
            if (!$aok) 
              {
                $this->errors[4] = "File &quot" . $this->path . $this->file["name"] . "&quot cannot be created.";
                unset($this->file['tmp_name']);
              }
            return $aok;
          }
        else
          {
            $this->errors[3] = "Only " . str_replace("|", " or ", $accept_type) . " files may be uploaded";
            return FALSE;
          }
      }
    /**
     * Convert Mac and/or PC line breaks to UNIX
     */
    
    function cleanup_text_file($file) 
      {
        # chr(13)  = CR (carridge return) = Macintosh
        # chr(10)  = LF (line feed)       = Unix
        # Win line break = CRLF
        $new_file = '';
        $old_file = '';
        $fcontents = file($file);
        while (list($line_num, $line) = each($fcontents)) 
          {
            $old_file.= $line;
            $new_file.= str_replace(chr(13) , chr(10) , $line);
          }
        if ($old_file != $new_file) 
          {
            # Open the uploaded file, and re-write it with the new changes
            $fp = fopen($file, "w");
            fwrite($fp, $new_file);
            fclose($fp);
          }
      }
    /**
     * Read file one line at a time. Returns FALSE on EOF or error
     */
    
    function read_file() 
      {
        if ($this->accepted) 
          {
            if (is_null($this->_fh)) 
              {
                if ($fh = fopen($this->file["tmp_name"], 'r')) 
                  {
                    $this->_fh = $fh;
                  }
                else
                  {
                    return FALSE;
                  }
              }
            if (feof($this->_fh)) 
              {
                fclose($this->_fh);
                return FALSE;
              }
            $buffer = fgets($this->_fh);
            return $buffer;
          }
        else
          {
            return FALSE;
          }
      }
    
    function __construct() 
      {
      }
  }
?>
