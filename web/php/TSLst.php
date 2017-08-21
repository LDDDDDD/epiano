<!DOCTYPE html>
<html>
<head> 
<meta charset="utf-8"> 

<script type="text/javascript" language="javascript" src="jquery.js"></script>  
<script type="text/javascript" language="javascript">  
	
		// 人工校核语音数据的按钮的处理
    function funYesBT(n) {  
        $.ajax({  
            url:"server.php",           //the page containing php script  
            type: "POST",               //request type  
            data:{action: n.value},  
            success:function(result){  
                alert(result);  
            }  
        });  
    }  
      
    function funNoBT(n) {  
        var url = "server.php";  
        var data = {  
            action : n.value  
        };  
        jQuery.post(url, data, callback);  
    }  
    function callback(data) {  
        alert(data);  
    }
    
    function funTest(n) {  
        alert(n.value);   
    }
    
		function UrlSearch(pageid) 
		{
		   var name,value; 
		   var str=location.href; //取得整个地址栏
		   var num=str.indexOf("?") 
		   str=str.substr(num+1); //取得所有参数   stringvar.substr(start [, length ]
		
		   var arr=str.split("&"); //各个参数放到数组里
		   for(var i=0;i < arr.length;i++){ 
		    num=arr[i].indexOf("="); 
		    if(num>0){ 
		     name=arr[i].substring(0,num);
		     value=arr[i].substr(num+1);
		     this[name]=value;
		     
		     if (name == "pageid")
		     {
		     	 return (int)$value;
		     } 
		    } 
		    
		    return 1;
		} 

    
</script>  


</head>	
<body>

<table width="100%" border="0">
<tr>
<td colspan="2" style="background-color:#FFA500;">
<h1 align="center">Teacher student list</h1>
</td>
</tr>

<!--
<form action="SongLst.php" method="post">
第<input type="text" name="pageid" value="1" width="100">页
<input type="submit">
</form>
-->

<?php

function Alert($Str,$Typ="back",$TopWindow="",$Tim=100){ 
  echo "<script>".chr(10); 
  if(!empty($Str)){ 
    echo "alert(\"Warning:\\n\\n{$Str}\\n\\n\");".chr(10); 
  } 
  echo "function _r_r_(){"; 
  $WinName=(!empty($TopWindow))?"top":"self"; 
  switch (StrToLower($Typ)){ 
  case "#": 
    break; 
  case "back": 
    echo $WinName.".history.go(-1);".chr(10); 
    break; 
  case "reload": 
    echo $WinName.".window.location.reload();".chr(10); 
    break; 
  case "close": 
    echo "window.opener=null;window.close();".chr(10); 
    break; 
  case "function": 
    echo "var _T=new function('return {$TopWindow}')();_T();".chr(10); 
    break; 
    //Die(); 
  Default: 
    if($Typ!=""){ 
      //echo "window.{$WinName}.location.href='{$Typ}';"; 
      echo "window.{$WinName}.location=('{$Typ}');"; 
    } 
  } 
  echo "}".chr(10); 
  //為防止Firefox不執行setTimeout 
  echo "if(setTimeout(\"_r_r_()\",".$Tim.")==2){_r_r_();}"; 
  if($Tim==100){ 
    echo "_r_r_();".chr(10); 
  }else{ 
    echo "setTimeout(\"_r_r_()\",".$Tim.");".chr(10); 
  } 
  echo "</script>".chr(10); 
  Exit(); 
}



// 根据时延确定时延字段的颜色
// input delay output str_color
function GetColorByDelay($delay) {
	$color = "#00FF00";
	if ($delay == 0)
  {
  	$color = "#C0C0C0";
  }
  else if ($delay < 300)
  {
  	$color = "#00FF00";
  }
  else if ($delay < 500)
  {
  	$color = "#00CC33";
  }
  else if ($delay < 800)
  {
  	$color = "#CCCC33";
  }
  else if ($delay < 1000)
  {
  	$color = "#CCFF33";
  }
  else if ($delay < 1500)
  {
  	$color = "#FF6633";
  }
  else
  {
  	$color = "#FF0033";
  }
  
  return $color;
}

// 根据ID查询用户名
function GetUserNameById($UsrId, $connp) {
	$UserNametmp = "";

	$rstmp = mysql_query( "select * from EpRegUsrTbl where UserId = " . $UsrId . ";", $connp);
	$rowtmp = mysql_fetch_array($rstmp);
	if ($rowtmp)
	{
		$UserNametmp  = iconv("GB2312","UTF-8//IGNORE",$rowtmp['UserName']);
	}
	/*if($rowtmp = mysql_fetch_array($rstmp))
		$UserNametmp  = iconv("GB2312","UTF-8//IGNORE",$rowtmp['UserName']);
		//break;
	}*/
	
	
	return $UserNametmp;
}


//echo "db connected 0";

$pagesize=15; //设定每一页显示的记录数
$conn = mysql_connect("localhost","root","ddshlmstzczx");
if (!$conn)
{
  die('Could not connect: ' . mysql_error());
}

$sqlx = "SET character_set_client='gbk'";
$result = mysql_query($sqlx);

$sqlx = "SET character_set_connection='gbk'";
$result = mysql_query($sqlx);

$sqlx = "SET character_set_results='gbk'";
$result = mysql_query($sqlx);

//echo "db connected";

mysql_select_db("epiano", $conn);


//mysql_select_db("sj",$conn);
$rs = mysql_query( "select * from TSTbl",$conn); //这里有第二个可选参数，指定打开的连接
//-----------------------------------------------------------------------------------------------//
//分页逻辑处理
//-----------------------------------------------------------------------------------------------
$tmpArr = mysql_fetch_array($rs);
$numAL = mysql_num_rows($rs);  //取得记录总数$rs
$pages=intval($numAL/$pagesize); //计算总页数
if ($numAL % $pagesize) $pages++;
//设置缺省页码
//↓判断“当前页码”是否赋值过
if (isset($_GET['page'])){ $page=intval($_GET['page']); }else{ $page=1; }//否则，设置为第一页
//↓计算记录偏移量
$offset=$pagesize*($page - 1);
//↓读取指定记录数
$result=mysql_query("select * from TSTbl order by TeacherUserId limit $offset,$pagesize",$conn);//取得—当前页—记录集！DateTime
$curNum = mysql_num_rows($result); //$curNum - 当前页实际记录数，for循环输出用

//////////////////////////////////////////////////////////////
// 翻页控制
//============================//
//  翻页显示 一               
//============================//
echo "<p>";  //  align=center
$first=1;
$prev=$page-1;   
$next=$page+1;
$last=$pages;
if ($page > 1)
{
echo "<a href='?page=".$first."'>首页</a>  ";
echo "<a href='?page=".$prev."'>上一页</a>  ";
}
if ($page < $pages)
{
echo "<a href='?page=".$next."'>下一页</a>  ";
echo "<a href='?page=".$last."'>尾页</a>  ";
}
//============================//
//  翻页显示 二               
//============================//
echo " | 共有".$pages."页(".$page."/".$pages.")";
//for ($i=1;$i< $page;$i++){echo "<a href='?page=".$i."'>[".$i ."]</a>  ";}  // 1-先输出当前页之前的
//if ($page > 0) echo "[".$page."]";; // 2-再输出当前页
//for ($i=$page+1;$i<=$pages;$i++){echo "<a href='?page=".$i."'>[".$i ."]</a>  ";}// 3-接着输出当前页之后
echo "转到第 <INPUT maxLength=3 size=3 value=".($page+1)." name=gotox> 页 <INPUT hideFocus onclick=\"location.href='?page=gotox.value';\" type=button value=Go name=cmd_goto>"; 
echo "</p>";



//////////////////////////////////////////////////////////////
// 表内容
echo "<table border='1' bgcolor=\"#F0F8FF\" align=\"center\" cellpadding=\"6\">
<tr>
<th>老师Id</th>
<th>学生Id</th>
<th>拜师时间</th>
</tr>";
while($row = mysql_fetch_array($result))
{
  //$wavelink = "<a href=\"{$row['AudioFilePathName']}\"></a>"
  //$utf8ASRResultStr = iconv("GB2312","UTF-8//IGNORE",$row['ASRResultStr']);
  
  //$UserNameMasked = substr($row['UserName'], 0, 3);
  //$UserNameMasked = $UserNameMasked."xxxxxx";
  $TeacherName  = GetUserNameById($row['TeacherUserId'], $conn);
  $StudentName  = GetUserNameById($row['StudentUserId'], $conn);
  //$TeacherName  = 'xx';
  //$StudentName  = 'yy';
  
  echo "<tr>";
                                                                       
  echo "<td width=\"100px\" align=\"center\">" . $TeacherName . "</td>";  
  echo "<td width=\"100px\" align=\"center\">" . $StudentName . "</td>";  
  echo "<td width=\"100px\" align=\"center\">" . $row['EstablishTime'] . "</td>";
/*
	//echo "<td><a href=\"{$row['AudioFilePathName']}\">" . "打开语音文件" . "</a></td>";  
	echo "<td><a href=\"audiofiles\\{$row['AudioFilePathName']}\">" . $row['AudioFilePathName'] . "</a></td>";  
	//echo "<td><a href=\"{$row['AudioFilePathName']}\">" . "打开语音文件" . "</a></td>";  
	
	$color_Delay = GetColorByDelay((int)$row['ASRE2EDurationInMs']);
  echo "<td bgcolor = \"{$color_Delay}\">" . $row['ASRE2EDurationInMs'] . "</td>";
  //echo "<td>" . $row['ASRE2EDurationInMs'] . "</td>";

	$color_Delay = GetColorByDelay((int)$row['ASRRecongitioanDurationInMs']);
  echo "<td bgcolor = \"{$color_Delay}\">" . $row['ASRRecongitioanDurationInMs'] . "</td>";
  //echo "<td>" . $row['ASRRecongitioanDurationInMs'] . "</td>";  
  
  // 根据数据库verify字段的值对按钮染色, 缺省灰色
  //$sqlx = "SELECT * FROM AllUserASRActTbl limit {$rowidstr}, {$pagerownum}";
  $tablename = "AllUserASRActTbl_".date('Y_m_d',strtotime($row['ASRDatetime']));
  $ASRDatatime = $row['ASRDatetime'];
  $whereclauseD = " where ASRDatetime = \"{$ASRDatatime}\" and UserName=\"{$row['UserName']}\"";
  //$whereclause = " where ASRDatetime = \\\"{$ASRDatatime}\\\" and UserName=\\\"{$row['UserName']}\\\"";
  $whereclause = " where ASRDatetime = '{$ASRDatatime}' and UserName='{$row['UserName']}'";
  $sqlx = "UPDATE {$tablename} set Verified = x ".$whereclause;
	//$result = mysql_query($sqlx);	
  $color_button_yes = "#C0C0C0";
  $color_button_no = "#C0C0C0";
  $color_button_del = "#C0C0C0";
  $verify = (int)$row['Verified'];
  if ($verify == 0)
  {
  	
  }
  else if ($verify == 1)
  {
  	$color_button_yes = "#00FF00";
  }
  else if ($verify == 2)
  {
  	$color_button_no = "#00FF00";
  }
  else if ($verify == 3)
  {
  	$color_button_del = "#00FF00";
  } 
	echo "<td><button  name=\"yes\" value=\"{$whereclause}\" onclick=\"funTest(this)\" style=\"background-color:{$color_button_yes}\">pos</button>&nbsp;<button name=\"no\" value=\"{$whereclause}\" style=\"background-color:{$color_button_no}\">neg</button>&nbsp;<button name=\"del\" value=\"{$whereclause}\" style=\"background-color:{$color_button_del}\">del</button>" . $row['Verified'] . "</td>";  
  //echo "<td><button  name=\"yes\" value=\"{$whereclause}\" style=\"background-color:{$color_button_yes}\">pos</button>&nbsp;<button name=\"no\" value=\"{$whereclause}\" style=\"background-color:{$color_button_no}\">neg</button>&nbsp;<button name=\"del\" value=\"{$whereclause}\" style=\"background-color:{$color_button_del}\">del</button>" . $tablename . "</td>";  
*/
  
  echo "</tr>";
}
echo "</table>";

// 释放内存
mysql_free_result($result);

 //echo db over;

mysql_close($conn);
?>


</body>
</html>