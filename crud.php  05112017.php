<?php
	date_default_timezone_set('America/Sao_Paulo');
	$time = date("H:i:s"); 
	$data = date("Y-m-d"); 
	$database="teste-esp";

	$numeroSerie = $_GET['ns'];
	$_dados = $_GET['dados'];
	$dados = explode(",",$_dados);


	$senhaSerie = (int)$dados[0];
	$numeroLote = (int)$dados[1];
	$temp = (int)$dados[2];
	$umid = (int)$dados[3];
	$tempAjst = (int)$dados[4];
	$umidAjst = (int)$dados[5];
	$numFalha = (int)$dados[6];
	$alarme = (int)$dados[7];
	$ventoinha = (int)$dados[8];
if($numeroSerie == null){echo "NS is null"; return;}

$conn = mysqli_connect("server","user","password", $database);
if($conn){
echo "CONECTADO";

if(read($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha)){

	$query = "SELECT time FROM NS".$numeroSerie." WHERE id=2";
    $result = mysqli_query($conn, $query);
    if (mysqli_num_rows($result) > 0) {
    	$row = mysqli_fetch_assoc($result);
    	$_time = $row["time"];
        $lasttime = explode(":",$_time);
	    $atualmin = gmdate("i");

	    if($atualmin != $lasttime[1])
	    {
	    	update($conn,1,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha);
            update($conn,2,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha);
	    	save($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha);
	    	echo "<br>ATUALIZA ID 2 E SALVA NOVO REGISTRO";
	    }
	    else
	    {
	    	update($conn,1,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha);
	    	echo "<br>ATUALIZA ID 1";
	    }
	    }	
	
}
	else{
		echo "<br>NAO ENCONTROU TABELA E CRIOU NOVA";
	   
	    create($conn,$numeroSerie);

		save($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha);

		save($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha);
	}

}
//======================== FUNÇOES CRUD ===================================================

function read($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha){

	$query = ("SELECT numero_serie FROM NS".$numeroSerie." WHERE numero_serie = '$numeroSerie'");
	if(mysqli_query($conn,$query)){
		return 1;
	}else{
		return 0;
	}
}

function create($conn,$numeroSerie){
	$query = "CREATE TABLE NS".$numeroSerie." (
    id INT NOT NULL AUTO_INCREMENT,
    data DATE NOT NULL,
    time TIME NOT NULL,
    numero_serie INT NOT NULL,
    senha_serie INT NOT NULL,
    numero_lote INT NOT NULL,
    temp INT,
    umid INT,
    temp_ajst INT,
    umid_ajst INT,
    num_falha INT,
    alarme INT,
    ventoinha INT,
    PRIMARY KEY (id)
	)";
	mysqli_query($conn,$query);
}

function update($conn,$id,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha){
	$query = "UPDATE NS".$numeroSerie." SET 
	data = '".$data."',
	time = '".$time."',
	numero_serie = '".$numeroSerie."',
	senha_serie = '".$senhaSerie."',
	numero_lote = '".$numeroLote."',
	temp = '".$temp."',
	umid = '".$umid."',
	temp_ajst = '".$tempAjst."',
	umid_ajst = '".$umidAjst."',
	num_falha = '".$numFalha."',
	alarme = '".$alarme."',
	ventoinha = '".$ventoinha."'
	WHERE id ='$id'";
	mysqli_query($conn,$query);
	
}

function insert($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha){
	$query = "UPDATE NS".$numeroSerie." SET 
	data = '".$data."',
	time = '".$time."',
	numero_serie = '".$numeroSerie."',
	senha_serie = '".$senhaSerie."',
	numero_lote = '".$numeroLote."',
	temp = '".$temp."',
	umid = '".$umid."',
	temp_ajst = '".$tempAjst."',
	umid_ajst = '".$umidAjst."',
	num_falha = '".$numFalha."',
	alarme = '".$alarme."',
	ventoinha = '".$ventoinha."'";
	mysqli_query($conn,$query);
	
}

function save($conn,$numeroSerie,$senhaSerie,$numeroLote,$data,$time,$temp,$umid,$tempAjst,$umidAjst,$numFalha,$alarme,$ventoinha){
	$query = "INSERT INTO NS".$numeroSerie."(
		data,
		time,
		numero_serie,
		senha_serie,
		numero_lote,
		temp,
		umid,
		temp_ajst,
		umid_ajst,
		num_falha,
		alarme,
		ventoinha		
			) VALUES (
			'".$data."',
			'".$time."',
			'".$numeroSerie."',
			'".$senhaSerie."',
			'".$numeroLote."',
			'".$temp."',
			'".$umid."',
			'".$tempAjst."',
			'".$umidAjst."',
			'".$numFalha."',
			'".$alarme."',
			'".$ventoinha."'
			)";
			
		mysqli_query($conn,$query);
}

if(mysqli_close($conn)){
	echo "<br><br>fechou conexao  ";
}

?>

