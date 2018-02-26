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
	
	/**
	 * Creates a new SensorSample given a sample type and a raw sample
	 * 
	 * @param type The SampleType of this sample
	 * @param sample The string containing the sample
	 */
	public SensorSample(SampleType type, String sample){
		this.type = type;
		this.sampleString = sample;
		this.value = type.parse(sample);
	}
	
	/**
	 * @return The SampleType of this sample
	 */
	public SampleType getType() { return type; }
	
	/**
	 * @return The value of this sample
	 */
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
