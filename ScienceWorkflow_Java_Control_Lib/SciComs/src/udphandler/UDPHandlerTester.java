package udphandler;

public class UDPHandlerTester {

	public static void main(String[] args) {
		UDPHandler handler = new UDPHandler("155.101.8.176", 9090);
		
		handler.send("DRIVE 0 0");
		handler.sendReceive("SENSE_LIST", new Handler());
		handler.sendReceive("SENSE_GET 0", new Handler());
		handler.sendReceive("SENSE_GET 1", new Handler());
		//handler.send("PAN 90 90");
		
		/*for(int i = 0; i < Short.MAX_VALUE; i++){
			while(!handler.send("DRIVE 0 " + Integer.toString(i)));
		}*/
		
		try {
			Thread.sleep(100);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		handler.kill();
		return;
	}

	
	
}

class Handler implements UDPResponseHandler{

	@Override
	public void handler(String response) {
		System.out.println(response);
	}
	
}