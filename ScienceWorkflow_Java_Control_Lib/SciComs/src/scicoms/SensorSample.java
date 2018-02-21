package scicoms;

/**
 *	Represents a singular data point from a sensor. 
 * 
 * @author Aaron Pabst
 */
public class SensorSample {
	private SampleType type;
	private Object value;
	private String sampleString;
	
	public SensorSample(SampleType type, String sample){
		this.type = type;
		this.sampleString = sample;
		this.value = type.parse(sample);
	}
	
	public SampleType getType() { return type; }
	
	public Object getValue() { return value; }
	
	@Override
	public String toString(){
		return sampleString;
	}

	@Override
	public boolean equals(Object o){
		return this.value.equals(((SensorSample)o).value);
	}
}
