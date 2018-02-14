package scicoms;

import java.util.Iterator;
import java.util.LinkedList;

/**
 * Implements a dynamically typed ring buffer.
 * 
 * Maintains a fixed number of objects. When the structure is at some pre-defined capacity,
 * kills the oldest entry to make room for a new entry.
 * 
 * @author Aaron Pabst
 *
 * @param <E> The type to be held within this data structure
 */
public class RingBuffer<E> {
	
	private LinkedList<E> structure; // The underlying data structure
	
	private int capacity; // The maximum number of entries in the buffer
	
	/**
	 * Initializes a new ring buffer with a specified capacity.
	 * 
	 * @param capacity
	 */
	public RingBuffer(int capacity){
		structure = new LinkedList<E>();
		this.capacity = capacity;
	}
	
	/**
	 * Adds a new element to the buffer. If the buffer is at capacity, deletes the oldest element.
	 * 
	 * @param val The value to add
	 */
	public void add(E val){
		if(structure.size() < capacity){
			structure.addLast(val);
		}
		else{
			structure.removeFirst();
			structure.addLast(val);
		}
	}
	
	/**
	 * Produces an iterator for the elements in the buffer starting at the oldest value and ending
	 * at the newest.
	 * 
	 * @return An iteration over the elements in the buffer
	 */
	public Iterable<E> getData(){
		return new Iterable<E>(){
			@Override
			public Iterator<E> iterator() {
				return structure.iterator();
			}
		};
	}
	
	/**
	 * @return The most recent element added to the buffer.
	 */
	public E getNewest(){
		return structure.getLast();
	}
	
	@Override
	public String toString(){
		String str = "";
		for(E value : structure){
			str = str.concat(value.toString());
		}
		return str;
	}
}
