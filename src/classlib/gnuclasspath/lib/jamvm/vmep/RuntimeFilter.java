package jamvm.vmep;

import java.lang.reflect.Method;

public abstract class RuntimeFilter{
	/**
	 * Filter pass.
	 */
	public static final int FILTER_PASS=0;

	/**
	 * Filter block for one method.
	 */
	public static final int FILTER_BLOCK=1;

	/**
	 * Filter block for one method and all subcalled methods.
	 */
	public static final int FILTER_RECURSIVE_BLOCK=2;
	/**
	 * Method is executed when filter is applied.
	 * @param	method	Method of instruction current filtering 
	 * @return		One of constants values FILTER_ALLOW, FILTER_BLOCK or FILTER_RECURSIVE_BLOCK.
	 */
	protected abstract int doFilter(Method method);
}
