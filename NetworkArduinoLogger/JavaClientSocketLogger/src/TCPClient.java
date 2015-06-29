
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class TCPClient {
	
	public static final String FILENAME = "temperatura.log";
	public static final String ARDUINO_IP = "192.168.0.6";
	public static final int ARDUINO_PORT = 1000;

	public static void run() throws UnknownHostException, IOException, InterruptedException{

		System.out.println("====== Rodando!!! ======");
		
		String sentence;
		Socket clientSocket = new Socket(ARDUINO_IP, ARDUINO_PORT); //Abre o socket com o Arduino com o dado IP e porta
		DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream()); //Pega o stream de envio de dados para o Arduino
		BufferedReader inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream())); //Pega o stream de recep��o de dados do Arduino

		outToServer.write("vai".getBytes()); //Envia alguma coisa s� pra come�ar a receber dados do Arduino

		FileOutputStream fos = new FileOutputStream(FILENAME, true); //Cria o arquivo de log com op��o de 'append', ou seja, escrever depois da �ltima linha sem apagar o que j� tinha no arquivo
		PrintWriter writer = new PrintWriter(fos); //Cria o objeto utilizado para escrever no arquivo de log

		long timeIni = System.currentTimeMillis(); //Pega o tempo utilizado no gerenciamento de timeout na comunica��o
		while(true){

			//Se n�o recebeu nada
			if(!inFromServer.ready()){

				//E passou dos tempo m�ximo de espera (10s), ent�o deu timeout
				if((System.currentTimeMillis() - timeIni) >= (10 * 1000)){
					break;
				}else{
					//Ainda n�o recebeu nada, espera um tempo (100ms) e checa novamente
					Thread.sleep(100);
					continue;
				}
			}

			//Chegou dados \o/, vamos l� !!!

			timeIni = System.currentTimeMillis(); //Atualiza o contador de tempo para o timeout

			//L� a linha de texto enviada pelo Arduino
			sentence = inFromServer.readLine();
			if(sentence == null) //Checa se veio null, indica��o de problemas de comunica��o
				break;

			if(!sentence.equals("")){ //S� pega as senten�as com conte�do v�lido
				writer.println(sentence); //Salva no arquivo de LOG
				writer.flush(); //For�a a escrita para acontecer agora
				
				System.out.println(sentence); //Mostra no console
			}
		}

		writer.close(); //Fecha o arquivo
		clientSocket.close(); //Fecha o socket
	}

	public static void main(String argv[]) throws Exception {

		while(true){
			try{
				run();
			}catch(Exception e){
				e.printStackTrace();
			}
		}

	}
}
