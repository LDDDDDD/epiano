<html><head>
<title>分页示例（php</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
</head>
<body>
<?php 
$pagesize=20; //设定每一页显示的记录数
$conn = mysql_connect("localhost","root","ddshlmstzczx");

$sqlx = "SET character_set_client='gbk'";
$result = mysql_query($sqlx);

$sqlx = "SET character_set_connection='gbk'";
$result = mysql_query($sqlx);

$sqlx = "SET character_set_results='gbk'";
$result = mysql_query($sqlx);

//echo "db connected";

mysql_select_db("epiano", $conn);


mysql_select_db("sj",$conn);
$rs = mysql_query( "select * from SongTbl",$conn); //这里有第二个可选参数，指定打开的连接
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
$rs=mysql_query("select * from SongTbl order by idx limit $offset,$pagesize",$conn);//取得—当前页—记录集！
$curNum = mysql_num_rows($rs); //$curNum - 当前页实际记录数，for循环输出用
?> 
<table border='1' bgcolor=\"#F0F8FF\" align=\"center\" cellpadding=\"6\">
<tr>
<th>索引号</th>
<th>曲名</th>
<th>作者</th>
</tr>"
<?php
while ($tmpArr = mysql_fetch_array($rs)) //提取一行，并循环判断
{
$i=0; 
// for($a=0;$a<$ColNum;$a++) //==for结束==
?> 
<tr>
 <td width="50%"><?= $tmpArr[1];  //$tmpArr["news_title"] ;  ?></td>
 <td width="50%"><?php echo $tmpArr[2];  //$tmpArr["news_cont"]; ?></td>
</tr>
<?php
}//==while结束==
?>
</table>
<?php
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
for ($i=1;$i< $page;$i++){echo "<a href='?page=".$i."'>[".$i ."]</a>  ";}  // 1-先输出当前页之前的
if ($page > 0) echo "[".$page."]";; // 2-再输出当前页
for ($i=$page+1;$i<=$pages;$i++){echo "<a href='?page=".$i."'>[".$i ."]</a>  ";}// 3-接着输出当前页之后
echo "转到第 <INPUT maxLength=3 size=3 value=".($page+1)." name=gotox> 页 <INPUT hideFocus onclick=\"location.href='?page=gotox.value';\" type=button value=Go name=cmd_goto>"; 
echo "</p>";
?>
</body>
</html>