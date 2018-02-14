package scicoms;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import javax.swing.Timer;

import udphandler.UDPHandler;
import udphandler.UDPResponseHandler;

/**
 * Abstracts away all tasks related to controlling and communicating with the SciWorkflow robot.
 * 
 * @author Aaron Pabst
 * @version 0.1
 */
public class Robot {
	private UDPHandler handler;
	
	private ArrayList<Sensor> sensors; 
	Lock sensorLock;
	
	private Timer updateTimer;
	
	private int sensorIndex; // Uses by the sense get receive handler to index into the sensor collection
	
	public Robot(String addr, int portno, int updateInterval){
		handler = new UDPHandler(addr, portno);
		sensors = new ArrayList<Sensor>();
		sensorLock = new ReentrantLock();
		
		senseList(); // Populate sensor collection
		
		sensorIndex = 0;
		
		ActionListener sensorUpdate = new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				// Iterate over sensor list and call sense get for each one
				updateAll();
			}
		};
		
		updateTimer = new Timer(updateInterval, sensorUpdate); // Set up an event to update the sensor history at a set time interval
	}
	
	public void drive(short theta, short speed){
		handler.send("DRIVE " + Short.toString(theta) + " " + Short.toString(speed));
	}
	
	public void pan(short x, short y){
		handler.send("PAN + " + Short.toString(x) + " " + Short.toString(y));
	}
	
	private void senseList(){
		handler.sendReceive("SENSE_LIST", new ReceiveSensorList());
	}
	
	private void senseGet(Sensor sensor){
		handler.sendReceive("SENSE_GET " + sensor.getId(), new ReceiveSensorSample());
	}
	
	private void updateAll(){
		for(Sensor s : sensors)
			senseGet(s);
	}
	
	private class ReceiveSensorList implements UDPResponseHandler{
		@Override
		public void handler(String response) {
			sensorLock.lock();
			sensors.clear();
			for(String s : response.split("\n")){
				sensors.add(new Sensor(s));
			}
			sensorLock.unlock();
		}
	}
	
	private class ReceiveSensorSample implements UDPResponseHandler{

		@Override
		public void handler(String response) {
			sensorLock.lock();
			sensors.get(sensorIndex).addSample(response);
			if(sensorIndex < sensors.size()-1)
				sensorIndex++;
			else
				sensorIndex = 0;
			sensorLock.unlock();
		}
		
	}
}