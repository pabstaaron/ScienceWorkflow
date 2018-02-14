package tests;

import static org.junit.Assert.*;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import org.junit.Test;

import udphandler.UDPHandler;
import udphandler.UDPResponseHandler;

public class UDPHandlerTester {

	private boolean dataRecv;
	private String data;
	
	@Test
	public void testSend() {
		System.out.println("Starting Server");
		RecvServer serv = new RecvServer(9091);
		serv.start();
		System.out.println("Server running");
		
		UDPHandler client = new UDPHandler("155.101.8.176", 9091);
		client.send("stimulus");
		
		for(long count = 0; count < 1000000000; count++){
			if(serv.recv){
				assertEquals(serv.data, "stimulus");
			}
		}
		fail("Timeout exceeded");
	}

	@Test
	public void testSendReceive() {
		SendRecvServer serv = new SendRecvServer(9092);
		serv.start();
		
		dataRecv = false;
		data = "";
		
		UDPHandler handler = new UDPHandler("155.101.8.176", 9092);
		handler.sendReceive("Stimulus", new UDPReceivedHandler());
		
		for(long count = 0; count < 1000000000; count++){
			if(dataRecv){
				assertEquals("response", data);
			}
		}
	}

	private class UDPReceivedHandler implements UDPResponseHandler {

		@Override
		public void handler(String response) {
			dataRecv = true;
			data = response;
		}
		
	}
	
	private String conditionData(byte[] data){
		String str = "";
		for(byte b : data){
			str = str.concat(String.valueOf((char)b));
			if(b == 0)
				break;
		}
		
		return str;
	}
	
	private class RecvServer extends Thread{
		int port;
		String data;
		boolean recv;
		
		RecvServer(int port){
			super();
			this.port = port;
			data = "";
			recv = false;
		}
		
		@Override
		public void run(){
			byte[] buff = new byte[1024];
			DatagramPacket pack = new DatagramPacket(buff, buff.length);
			
			try {
				DatagramSocket sock = new DatagramSocket(port);
				sock.receive(pack);
				data = conditionData(buff);
				recv = true;
				sock.close();
				
			} catch (SocketException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	private class SendRecvServer extends Thread{
		int port;
		String data;
		boolean recv;
		
		SendRecvServer(int port){
			super();
			this.port = port;
			data = "";
			recv = false;
		}
		
		@Override
		public void run(){
			byte[] buff = new byte[1024];
			DatagramPacket pack = new DatagramPacket(buff, buff.length);
			
			try {
				DatagramSocket sock = new DatagramSocket(port);
				sock.receive(pack);
				data = conditionData(buff);
				recv = true;
				
				String dat = "response";
				DatagramPacket sPack = new DatagramPacket(dat.getBytes(), dat.length(), pack.getAddress(), pack.getPort());
				
				sock.send(sPack);
				
				sock.close();
				
			} catch (SocketException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
