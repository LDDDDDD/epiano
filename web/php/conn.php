<?php
function mysql_open()
{
define(ALL_PS,"PHP100");

$mysql_servername="121.42.153.237";
$mysql_username="root";
$mysql_password="ddshlmstzczx";
$mysql_dbname="epiano";

$conn=mysql_connect($mysql_servername ,$mysql_username ,$mysql_password);
      mysql_query("set names UTF8");
      mysql_select_db($mysql_dbname , $conn);
	  return $conn;
}
?>