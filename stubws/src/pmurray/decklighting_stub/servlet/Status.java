package pmurray.decklighting_stub.servlet;

import java.io.IOException;
import java.io.Writer;
import java.util.Random;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class Master
 */
@WebServlet({ "/on", "/off", "/out", "/density", "/colour", "/interp", "/effect" })

public class Status extends HttpServlet {
	private static final long serialVersionUID = 1L;

	Random rnd = new Random();

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.setContentType("application/json");
		Writer w = response.getWriter();
		if ("all".equals(request.getParameter("room")))
			sendMaster(w);
		else
			sendRoom(w);
		w.flush();
		w.close();
	}

	static String[] st = { "on", "off", "out" };
	static String[] interp = { "linear", "hueup", "huedown", "huenear", "huefar" };
	static String[] effect = { "rainbow", "theatre", "plasma" };

	void sendMaster(Writer w) throws IOException {
		w.write("{ \"status\": \"");
		w.write(st[rnd.nextInt(2)]);
		w.write("\", \"brightness\": ");
		w.write(Integer.toString(rnd.nextInt(256)));
		w.write("}");
	}

	void sendRoom(Writer w) throws IOException {
		// { status: on/off/out; density: n; c1=rgb; c2=rgb; interp=_; effect=_; }
		w.write("{" //
				+ " \"status\": \"");
		w.write(st[rnd.nextInt(3)]);
		w.write("\", \"density\": ");
		w.write(Integer.toString(rnd.nextInt(10) + 1));
		w.write(",\"c1\": \"#");
		for (int i = 0; i < 6; i++)
			w.write(Integer.toHexString(rnd.nextInt(16)));
		w.write("\",\"c2\": \"#");
		for (int i = 0; i < 6; i++)
			w.write(Integer.toHexString(rnd.nextInt(16)));
		w.write("\", \"interp\": \"");
		w.write(interp[rnd.nextInt(3)]);
		w.write("\", \"effect\": \"");
		w.write(effect[rnd.nextInt(3)]);
		w.write("\"");
		w.write("}");
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		doGet(request, response);
	}

}
