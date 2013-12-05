package jamvm.vmep;

import java.lang.reflect.Method;

public abstract class RuntimeFilter{
	public static final int FILTER_CONTINUE=0;
	public static final int FILTER_STOP=1;
	public static final int FILTER_RECURSIVE_BLOCK=2;
	protected abstract int doFilter(Method method);
}
