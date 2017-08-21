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
    
</script>  


</head>	
<body>

<table width="100%" border="0">
<tr>
<td colspan="2" style="background-color:#FFA500;">
<h1 align="center">eSpace语音识别业务流水</h1>
</td>
</tr>

<form action="index.php" method="post">
第<input type="text" name="pageid" value="1" width="100">页
<!--<input type="submit" value="上页" class="button1" title="上页" />-->
<!--<input type="submit" value="下页" class="button2" title="下页" />-->
<input type="submit">
</form>


<?php

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

//echo "db connected 0";
$con = mysql_connect("localhost","root","ddshlmstzczx");
if (!$con)
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

mysql_select_db("ASR_DB", $con);



$sqlx0 = $_POST["pageid"];
//echo $sqlx0;

$pagerownum = 30;
$rowid = (int)$_POST["pageid"];
$rowid=$rowid-1; // 页号从０开始
if ($rowid < 0)
{
	$rowid = 0;
}
$rowid = $rowid * $pagerownum;
$rowidstr = strval($rowid);

// 查询记录总数$TotalRowCount
//$sqlx = "SELECT count(ASRDatetime) FROM AllUserASRActTbl";
$sqlx = "SELECT * FROM AllUserASRActTbl";
$result=mysql_query($sqlx);
$count=mysql_num_rows($result);
// 释放内存
mysql_free_result($result);
echo "共" . $count . "条记录";

//$sqlx = "SELECT * FROM AllUserASRActTbl limit {$_POST["pageid"] * 30}, 30";
//echo $sqlx;
$sqlx = "SELECT * FROM AllUserASRActTbl order by ASRDatetime desc limit {$rowidstr}, {$pagerownum}";
$result = mysql_query($sqlx);

echo "<table border='1' bgcolor=\"#F0F8FF\" align=\"center\" cellpadding=\"6\">
<tr>
<th>时间</th>
<th>用户名</th>
<th>语音识别</th>
<th>原始语音</th>
<th>降噪语音</th>
<th>端到端时长</th>
<th>识别时长</th>
<th>人工校核</th>
</tr>";

while($row = mysql_fetch_array($result))
{
  //$wavelink = "<a href=\"{$row['AudioFilePathName']}\"></a>"
  $utf8ASRResultStr = iconv("GB2312","UTF-8//IGNORE",$row['ASRResultStr']);
  
  $UserNameMasked = substr($row['UserName'], 0, 3);
  $UserNameMasked = $UserNameMasked."xxxxxx";
  
  echo "<tr>";
  //echo "<td>" . $row['ASRDatetime'] . "</td>";
	echo "<td>" . $row['ASRDatetime'] . "</td>";  
  echo "<td>" . $UserNameMasked . "</td>";
  echo "<td>" . $utf8ASRResultStr . "</td>";
  if (!empty($row['OrgAudioFilePathName']))
  {
  	echo "<td><a href=\"audiofiles\\{$row['OrgAudioFilePathName']}\">" . "原始语音" . "</a></td>";  
  }
  else
  {
  	echo "<td>" . "空" . "</td>";  
  }	
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
  echo "</tr>";
}
echo "</table>";

// 释放内存
mysql_free_result($result);

 //echo db over;

mysql_close($con);
?>


</body>
</html>