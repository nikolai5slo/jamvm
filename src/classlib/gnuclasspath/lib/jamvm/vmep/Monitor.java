package jamvm.vmep;

import java.util.Arrays;
import java.util.Hashtable;
import java.lang.reflect.Method;

public class Monitor{
	private int[] bcc=new int[201];
	public native void start();
	public native void stop();

	private Hashtable<Method,int[]>opcount=new Hashtable<Method,int[]>();

	private int[] getArrayCount(Method m){
		int[] opcodes=opcount.get(m);
		if(opcodes==null){
			opcodes=new int[201];
			opcount.put(m,opcodes);
		}
		opcodes[3]=200;
		return opcodes;	
	}
	public int getCountFor(Method m, int opcode){
		return opcount.get(m)[opcode];
	}
}
