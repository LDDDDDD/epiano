<!DOCTYPE html>
<html>
<head> 
<meta charset="utf-8"> 


<?php
    /**
     * Email net.webjoy@gmail.com
     * author jackluo
     * 2014.11.21
     * 
     */

    //*
    function curl_post($url, $data, $header = array()){
            if(function_exists('curl_init')) {
                $ch = curl_init();
                curl_setopt($ch, CURLOPT_URL, $url);
                if(is_array($header) && !empty($header)){
                    $set_head = array();
                    foreach ($header as $k=>$v){
                        $set_head[] = "$k:$v";
                    }
                    curl_setopt($ch, CURLOPT_HTTPHEADER, $set_head);
                }
                curl_setopt($ch, CURLOPT_HEADER, 0);
                curl_setopt($ch, CURLOPT_POST, 1);
                curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
                curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
                curl_setopt($ch, CURLOPT_TIMEOUT, 1);// 1s to timeout.
                $response = curl_exec($ch);
                if(curl_errno($ch)){
                    //error
                    return curl_error($ch);
                }
                $reslut = curl_getinfo($ch);
                print_r($reslut);
                curl_close($ch);
                $info = array();
                if($response){
                    $info = json_decode($response, true);
                }
                return $info;
            } else {
                throw new Exception('Do not support CURL function.');
            }
    }
    //*/
    //  
    function api_notice_increment($url, $data)
    {
        $ch = curl_init();        
        curl_setopt($ch, CURLOPT_HEADER,0);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, 1);
//        $data = http_build_query($data);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        //curl_file_create
    //    $result =  curl_exec($ch);
        $lst['rst'] = curl_exec($ch);
        $lst['info'] = curl_getinfo($ch);
        curl_close($ch); 
    
        return $lst;
    //    return $result;
    }

     /**
         *  curl文件上传
         *  @var  struing  $r_file  上传文件的路劲和文件名  
         *     
         */
    /*     
    function upload_file($url,$r_file)
     {
        $file = array("fax_file"=>'@'.$r_file,'type'=>'image/jpeg');//文件路径，前面要加@，表明是文件上传.
        $curl = curl_init();
        curl_setopt($curl, CURLOPT_URL,$url);
        curl_setopt($curl,CURLOPT_POST,1);
        curl_setopt($curl,CURLOPT_POSTFIELDS,$file);
        curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($curl, CURLOPT_HEADER, 0);
        $result = curl_exec($curl);  //$result 获取页面信息 
        curl_close($curl);
        echo $result ; //输出 页面结果
   }*/
    
   function upload_file($url,$filename,$path,$type){
        $data = array(
            'pic'=>'@'.realpath($path).";type=".$type.";filename=".$filename
        );
        $ch = curl_init();
   //设置帐号和帐号名

   curl_setopt($ch, CURLOPT_USERPWD, 'root:huawei123' );


        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, true );
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch, CURLOPT_HEADER, false);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        // curl_getinfo($ch);
        $return_data = curl_exec($ch);
        curl_close($ch);
        echo $return_data;       
   }

/*
　　// php 5.5 以后请用以下函数
function upload_file($url,$filename,$path,$type){
　　$data = array(
　　　　'pic'=>new CURLFile(realpath($path))
　　);
　　$ch = curl_init();

   //也可以用以下注释掉的不用改代码，觉得新版的可以省下点代码，看个人

   //curl_setopt($ch, CURLOPT_SAFE_UPLOAD, false);

   //设置帐号和帐号名

   curl_setopt($ch, CURLOPT_USERPWD, 'joe:secret' );
　　curl_setopt($ch, CURLOPT_URL, $url);
　　curl_setopt($ch, CURLOPT_POST, true );
　　curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
　　curl_setopt($ch, CURLOPT_HEADER, false);
　　curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
　　// curl_getinfo($ch);
　　$return_data = curl_exec($ch);
　　curl_close($ch);
　　echo $return_data; 
}
*/




    if ($_POST) {
        $url = 'http://platform.com/upload/image';
        //
        $path = $_SERVER['DOCUMENT_ROOT'];
/*
        print_r($_FILES);
        exit;
*/
        //$filename = $path."/232.jpg";
        //upload tmp
        $tmpname = $_FILES['fname']['name'];
        $tmpfile = $_FILES['fname']['tmp_name'];
        $tmpType = $_FILES['fname']['type'];
//        echo $tmpType;

				echo $curl. ",";
				echo $path. ",";
				echo $tmpname. ",";
				echo $tmpfile. ",";
				echo $tmpType. ",";

        upload_file($url,$tmpname,$tmpfile,$tmpType);
        /*
        $data = array(
                'path'=>"@$path/232.jpg",
                'name'=>'h'
        );
        */
        //'pic'=>'@/tmp/tmp.jpg', 'filename'=>'tmp'
        //$data = array('pic'=>"@$filename", 'filename'=>'tmp');
/*
        $data = array(
            'uid'    =>    10086,
            'pic'    =>    '@$tmpfile'.';type='.$tmpType
        );
        $info = api_notice_increment($url, $data);
*/
        //$info = curl_post($url, $data);
        //$info = api_notice_increment($url, $data);
        //upload_file($url,$tmpfile);
        //print_r($info);
        exit;
/*
        $file = 'H:\www\test\psuCARGLSPA-pola.jpg'; //要上传的文件
        $src = upload_curl_pic($file);
        echo $src;
*/
    }    
?>

<form action="upload.php" enctype="multipart/form-data"  method="post">
  <p>UpLoad: <input type="text" name="fname" /></p>
  <p>UpLoad: <input type="file" name="fname" /></p>

  <input type="submit" value="Submit" />
</form>

</body>
</html>