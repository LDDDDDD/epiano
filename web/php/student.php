<?php
session_start();
include("conn.php");

$userid=str_replace(" ","",$_POST[userid]);

$conn = mysql_open();
if(empty($conn))
{
   die("数据库连接失败");
}
else
{   
	 
   
   $test="select l.StudentUserId,u.UserName from EpRegUsrTbl u,TSLessionTbl l where u.UserId = l.StudentUserId and l.TeacherUserId = 10";
   $result = mysql_query($test);
   
   mysql_query("set names utf8");
  
	//查询
	//while ($row = mysql_fetch_array($result)) { 
	//	echo $row['longitude'];
	//	echo $row['latitude'];
	//	echo $row['address'];
	
	//}
	
	$arr = array();

	while($row = mysql_fetch_array($result, MYSQL_ASSOC)){   //查询出来sql   
	$arr[] = $row;                                   //将查询出来的结果赋给数组$arr
	}

	$str = json_encode($arr);                           //将数组转化为json格式的字符串  
	      
	echo json_encode($arr, JSON_UNESCAPED_UNICODE);
	
	
	//显示结果
	mysql_free_result($result);

	//关闭连接
	mysql_close();
} 
?>