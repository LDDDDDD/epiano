<html><head>
<title>��ҳʾ����php</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
</head>
<body>
<?php 
$pagesize=20; //�趨ÿһҳ��ʾ�ļ�¼��
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
$rs = mysql_query( "select * from SongTbl",$conn); //�����еڶ�����ѡ������ָ���򿪵�����
//-----------------------------------------------------------------------------------------------//
//��ҳ�߼�����
//-----------------------------------------------------------------------------------------------
$tmpArr = mysql_fetch_array($rs);
$numAL = mysql_num_rows($rs);  //ȡ�ü�¼����$rs
$pages=intval($numAL/$pagesize); //������ҳ��
if ($numAL % $pagesize) $pages++;
//����ȱʡҳ��
//���жϡ���ǰҳ�롱�Ƿ�ֵ��
if (isset($_GET['page'])){ $page=intval($_GET['page']); }else{ $page=1; }//��������Ϊ��һҳ
//�������¼ƫ����
$offset=$pagesize*($page - 1);
//����ȡָ����¼��
$rs=mysql_query("select * from SongTbl order by idx limit $offset,$pagesize",$conn);//ȡ�á���ǰҳ����¼����
$curNum = mysql_num_rows($rs); //$curNum - ��ǰҳʵ�ʼ�¼����forѭ�������
?> 
<table border='1' bgcolor=\"#F0F8FF\" align=\"center\" cellpadding=\"6\">
<tr>
<th>������</th>
<th>����</th>
<th>����</th>
</tr>"
<?php
while ($tmpArr = mysql_fetch_array($rs)) //��ȡһ�У���ѭ���ж�
{
$i=0; 
// for($a=0;$a<$ColNum;$a++) //==for����==
?> 
<tr>
 <td width="50%"><?= $tmpArr[1];  //$tmpArr["news_title"] ;  ?></td>
 <td width="50%"><?php echo $tmpArr[2];  //$tmpArr["news_cont"]; ?></td>
</tr>
<?php
}//==while����==
?>
</table>
<?php
//============================//
//  ��ҳ��ʾ һ               
//============================//
echo "<p>";  //  align=center
$first=1;
$prev=$page-1;   
$next=$page+1;
$last=$pages;
if ($page > 1)
{
echo "<a href='?page=".$first."'>��ҳ</a>  ";
echo "<a href='?page=".$prev."'>��һҳ</a>  ";
}
if ($page < $pages)
{
echo "<a href='?page=".$next."'>��һҳ</a>  ";
echo "<a href='?page=".$last."'>βҳ</a>  ";
}
//============================//
//  ��ҳ��ʾ ��               
//============================//
echo " | ����".$pages."ҳ(".$page."/".$pages.")";
for ($i=1;$i< $page;$i++){echo "<a href='?page=".$i."'>[".$i ."]</a>  ";}  // 1-�������ǰҳ֮ǰ��
if ($page > 0) echo "[".$page."]";; // 2-�������ǰҳ
for ($i=$page+1;$i<=$pages;$i++){echo "<a href='?page=".$i."'>[".$i ."]</a>  ";}// 3-���������ǰҳ֮��
echo "ת���� <INPUT maxLength=3 size=3 value=".($page+1)." name=gotox> ҳ <INPUT hideFocus onclick=\"location.href='?page=gotox.value';\" type=button value=Go name=cmd_goto>"; 
echo "</p>";
?>
</body>
</html>