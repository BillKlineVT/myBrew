package com.billklinefelter.myBrewTests.GUITest;
import org.sikuli.script.*;
import static org.junit.Assert.*;
//import org.sikuli.script.ImagePath;


public class myBrewGUITest {

	public static void main(String[] args) throws FindFailed {
		ImagePath.setBundlePath("./images/");
		Screen s = new Screen();
		//Region r = new Region(0,0,100,700);
		//Match m = r.find("mainTab_selected.png");
		//System.out.print("Found at X: " + m.getX() + " Y: " + m.getY());
		
		//s.setAutoWaitTimeout(20.0);
		System.out.print("current timeout: " + s.getAutoWaitTimeout());
		assertNotNull("test exist", s.exists(new Pattern("cooler.png").similar(0.8f)));
		s.click(new Pattern("cooler.png").similar(0.8f));
		Pattern maintab = new Pattern("mainTab_selected.png").similar(0.8f);
		assertNotNull("Verify correct tab at start", s.exists(maintab));
		s.click(maintab);
		s.click("settingsTab_deselected.png");
		assertNotNull("Verify correct tab", s.exists("mainTab_deselected.png"));
		assertNotNull("Verify correct tab", s.exists("settingsTab_selected.png"));

		//Mouse.move(20,50);

	}

}
