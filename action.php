<?php
include 'DB.php';
    $db = new DB(); 
    $tblName = 'info';
    
    if(isset($_REQUEST['type']) && !empty($_REQUEST['type'])){
        $type = $_REQUEST['type'];
        switch($type){
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

            case "viewTemp":
                $cond = array(
                    "select"   => "output", 
                    "order_by" => "id desc",
                    "limit"    => "1"
                );
                $records = $db->getRows($tblName,$cond);

                if($records){
                    $data['records'] = $db->getRows($tblName,$cond);
                    $data['status'] = 'OK';
                }else{
                    $data['records'] = array();
                    $data['status'] = 'ERR';
                }
                echo json_encode($data);
                break;

            case "add":
                if(!empty($_POST['data'])){
                    $userData = array(
                        'station' => $_POST['data']['station'],
                        'remarks' => $_POST['data']['remarks']
                    );
                    $insert = $db->insert($tblName,$userData);
                    if($insert){
                        $data['data'] = $insert;
                        $data['status'] = 'OK';
                        $data['msg'] = 'User data has been added successfully.';
                    }else{
                        $data['status'] = 'ERR';
                        $data['msg'] = 'Some problem occurred, please try again.';
                    }
                }else{
                    $data['status'] = 'ERR';
                    $data['msg'] = 'Some problem occurred, please try again.';
                }
                echo json_encode($data);
                break;

            case "edit":
                if(!empty($_POST['data'])){ 
                    $userData = array(
                        'id' => $_POST['data']['id'],
                    'station' => $_POST['data']['station'],
                    'remarks' => $_POST['data']['remarks']
                    );
                    $condition = array('id' => $_POST['data']['id']);
                    $update = $db->update($tblName,$userData,$condition);
                    if($update){
                        $data['status'] = 'OK';
                        $data['msg'] = 'User data has been updated successfully.';
                    }else{
                        $data['status'] = 'ERR';
                        $data['msg'] = 'Some problem occurred, please try again.';
                    }
                }else{
                    $data['status'] = 'ERR';
                    $data['msg'] = 'Some problem occurred, please try again.';
                }
                echo json_encode($data);
                break;

             case "delete":
                if(!empty($_POST['id'])){
                    $condition = array('id' => $_POST['id']);
                    $delete = $db->delete($tblName,$condition);
                    if($delete){
                        $data['status'] = 'OK';
                        $data['msg'] = 'User data has been deleted successfully.';
                    }else{
                        $data['status'] = 'ERR';
                        $data['msg'] = 'Some problem occurred, please try again.';
                    }
                }else{
                    $data['status'] = 'ERR';
                    $data['msg'] = 'Some problem occurred, please try again.';
                }
                echo json_encode($data);
                break;

                
            default:
                echo '{"status":"INVALID"}';
        }
}