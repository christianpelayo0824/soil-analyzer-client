<?php
include 'db.php';
$db = new DB();
$tblName = 'info';

function console_log( $tempdata ){
  echo '<script>';
  echo 'console.log('. json_encode( $tempdata ) .')';
  echo '</script>';
}
console_log($type);

if(isset($_REQUEST['type']) && !empty($_REQUEST['type'])){
    $type = $_REQUEST['type'];
    
    switch($type){
        case "viewTemp":
            $temp = $db->getRows();
            $cond = array(
                "select"   => "remark", 
                "order_by" => "id",
                "limit"    => "1"
            );

            if($records){
                $data['temp'] = $db->getRows($tblName,$cond);
                $data['status'] = 'OK';
            }else{
                $data['temp'] = array();
                $data['status'] = 'ERR';
            }
            echo json_encode($data);
            break;

        case "view":
            $records = $db->getRows($tblName);
            if($records){
                $data['records'] = $db->getRows($tblName);
                $data['status'] = 'OK';
            }else{
                $data['records'] = array();
                $data['status'] = 'ERR';
            }
            echo json_encode($data);
            break;
        
        default:
            echo '{"status":"INVALID"}';
    }
}

?>