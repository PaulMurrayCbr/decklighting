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
@WebServlet({ "/on", "/off", "/out", "/density", "/colour", "/interp", "/effect", "/brightness", "/" })

public class Status extends HttpServlet {
	private static final long serialVersionUID = 1L;

	Random rnd = new Random();

	static String[] st = { "on", "off", "out" };
	static String[] interp = { "linear", "hueup", "huedown", "huenear", "huefar", "huerbow", "huexrbow" };
	static String[] effect = { "rainbow", "theatre", "plasma" };

	
	int effectn = 0;
	int interpn = 0;

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		try {
			Thread.sleep(500);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		response.setContentType("application/json");
		response.addHeader("Access-Control-Allow-Origin", "*");
		response.addHeader("Access-Control-Allow-Method", "GET, POST");
		Writer w = response.getWriter();
		if ("all".equals(request.getParameter("room")))
			sendMaster(w);
		else {
			sendRoom(w);
			if(++effectn>=effect.length) effectn= 0;
			if(++interpn>=interp.length) interpn= 0;
		}
		w.flush();
		w.close();
	}

	void sendMaster(Writer w) throws IOException {
		w.write("{ \"status\": \"");
		w.write(st[rnd.nextInt(2)]);
		w.write("\", \"brightness\": ");
		w.write(Integer.toString(rnd.nextInt(10)));
		w.write("}");
	}

	
	void sendRoom(Writer w) throws IOException {
		// { status: on/off/out; density: n; c1=rgb; c2=rgb; interp=_; effect=_; }
		w.write("{" //
				+ " \"status\": \"");
		w.write(st[rnd.nextInt(3)]);
		w.write("\", \"density\": ");
		w.write(Integer.toString(rnd.nextInt(4)*2 + 1));
		w.write(",\"c1\": \"#");
		for (int i = 0; i < 6; i++)
			w.write(Integer.toHexString(rnd.nextInt(16)));
		w.write("\",\"c2\": \"#");
		for (int i = 0; i < 6; i++)
			w.write(Integer.toHexString(rnd.nextInt(16)));
		w.write("\", \"interp\": \"");
		w.write(interp[interpn]);
		
		w.write("\", \"effect\": \"");
		w.write(effect[effectn]);
		w.write("\"");
		
		w.write(", \n\t\"effectData\": {");
		
		switch(effectn) {
		case 0:
			
			
			
			break;
		case 1:
			break;
		case 2:
			break;
		}
		
		w.write("\t\t}");
		
		
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
