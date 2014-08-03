package jamvm.vmep;

import java.util.Arrays;
import java.util.LinkedList;

public abstract class Monitor{
	static{
		initMonitors(RuntimeFilter.class);
	}
	public Monitor(){
		if(this instanceof InstructionMonitor) type=1;
		else if(this instanceof MemoryMonitor) type=2;
	}
	private static native void initMonitors(Class c);

	private static final int STOPPED=0;
	private static final int RUNNING=1;
	private static final int IDLE=2;
	private static final int IDLER=3;

	private int status=STOPPED; 
	private int type=0;

	private native void mstart();
	private native void mstop();

	/**
	 * Starts monitoring of this monitor.
	 */
	public void start(){
		status=IDLE;
		mstart();
	}

	/**
	 * Stops monitoring of this monitor.
	 */
	public void stop(){
		mstop();
		status=STOPPED;
	}
	
	/**
	 * Checks if monitor is running/monitoring.
	 *
	 * @return		Returns true if is running and false if not.
	 */
	public boolean isRunning(){
		return status>0;
	}

	/**
	 * Method is executed on each java instruction execution.
	 */
	protected abstract void onExecute();
	
	private RuntimeFilter[] rFilters=new RuntimeFilter[0];

	/**
	 * Adds runtime filter to monitor. All added filters are applied on runtime, for better preformance.
	 * @param	filter	Filter to add to monitor.
	 * @see 	RuntimeFilter
	 */
	public void addRuntimeFilter(RuntimeFilter filter){
		rFilters=Arrays.copyOf(rFilters,rFilters.length+1);	
		rFilters[rFilters.length-1]=filter;
	}

	/**
	 * Removes runtime filter from monitor.
	 * @param	filter	Filter to remove from monitor.
	 * @see RuntimeFilter
	 */
	public void removeRuntimeFilter(RuntimeFilter filter){
		LinkedList<RuntimeFilter> list = new LinkedList<RuntimeFilter>();
		for(RuntimeFilter f:rFilters) if(f!=filter) list.add(f);
		rFilters = list.toArray(new RuntimeFilter[0]);
	}
}
