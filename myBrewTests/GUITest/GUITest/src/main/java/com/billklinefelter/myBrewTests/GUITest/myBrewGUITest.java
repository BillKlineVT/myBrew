package com.billklinefelter.myBrewTests.GUITest;
import org.sikuli.script.*;
import static org.junit.Assert.*;
//import org.sikuli.script.ImagePath;


public class myBrewGUITest {

	public static boolean clickTab(Screen screen_var, Pattern selected, Pattern deselected) {
		if (screen_var.exists(selected) != null) {
			try {
				screen_var.click(selected);
			} catch (FindFailed e) {
				// TODO Auto-generated catch block 
				e.printStackTrace();
			}
			return true;
		}
		else if (screen_var.exists(deselected) != null) {
			try {
				screen_var.click(deselected);
			} catch (FindFailed e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return true;
		}
		else {
			return false;
		}
	}
	
	public static boolean clickCheckbox(Screen screen_var, Pattern checkBoxWithLabel, int offset_x, int offset_y) {
		
		if (screen_var.exists(checkBoxWithLabel) != null) {
			try {
				screen_var.click(checkBoxWithLabel.targetOffset(offset_x, offset_y));
			} catch (FindFailed e) {
				// TODO Auto-generated catch block 
				e.printStackTrace();
			}
			return true;
		}
		
		return false;
	}
	
	public static boolean setState(Screen screen_var, String state) {
		
		double similarity = 0.8;
		
		switch (state) {
		case "main":
			Pattern mainTab_selected = new Pattern("mainTab_selected.png").similar(similarity);
			Pattern mainTab_deselected = new Pattern("mainTab_deselected.png").similar(similarity);
			return clickTab(screen_var, mainTab_selected, mainTab_deselected);
		case "settings":
			Pattern settingsTab_selected = new Pattern("settingsTab_selected.png").similar(similarity);
			Pattern settingsTab_deselected = new Pattern("settingsTab_deselected.png").similar(similarity);
			return clickTab(screen_var, settingsTab_selected, settingsTab_deselected);
		case "recipe":
			Pattern recipeTab_selected = new Pattern("recipeTab_selected.png").similar(similarity);
			Pattern recipeTab_deselected = new Pattern("recipeTab_deselected.png").similar(similarity);
			return clickTab(screen_var, recipeTab_selected, recipeTab_deselected);
		case "hlt":
			Pattern hltTab_selected = new Pattern("hltTab_selected.png").similar(similarity);
			Pattern hltTab_deselected = new Pattern("hltTab_deselected.png").similar(similarity);
			return clickTab(screen_var, hltTab_selected, hltTab_deselected);
		case "mlt":
			Pattern mltTab_selected = new Pattern("mltTab_selected.png").similar(similarity);
			Pattern mltTab_deselected = new Pattern("mltTab_deselected.png").similar(similarity);
			return clickTab(screen_var, mltTab_selected, mltTab_deselected);
		case "boil":
			Pattern boilTab_selected = new Pattern("boilTab_selected.png").similar(similarity);
			Pattern boilTab_deselected = new Pattern("boilTab_deselected.png").similar(similarity);
			return clickTab(screen_var, boilTab_selected, boilTab_deselected);
		case "chill":
			Pattern chillTab_selected = new Pattern("chillTab_selected.png").similar(similarity);
			Pattern chillTab_deselected = new Pattern("chillTab_deselected.png").similar(similarity);
			return clickTab(screen_var, chillTab_selected, chillTab_deselected);
		case "clean":
			Pattern cleanTab_selected = new Pattern("cleanTab_selected.png").similar(similarity);
			Pattern cleanTab_deselected = new Pattern("cleanTab_deselected.png").similar(similarity);
			return clickTab(screen_var, cleanTab_selected, cleanTab_deselected);
		case "timers":
			Pattern timersTab_selected = new Pattern("timersTab_selected.png").similar(similarity);
			Pattern timersTab_deselected = new Pattern("timersTab_deselected.png").similar(similarity);
			return clickTab(screen_var, timersTab_selected, timersTab_deselected);
		case "logs":
			Pattern logsTab_selected = new Pattern("logsTab_selected.png").similar(similarity);
			Pattern logsTab_deselected = new Pattern("logsTab_deselected.png").similar(similarity);
			return clickTab(screen_var, logsTab_selected, logsTab_deselected);
		case "checklist":
			Pattern checklistTab_selected = new Pattern("checklistTab_selected.png").similar(similarity);
			Pattern checklistTab_deselected = new Pattern("checklistTab_deselected.png").similar(similarity);
			boolean result = clickTab(screen_var, checklistTab_selected, checklistTab_deselected);
			return result;
		default:
			return false;
		}
	}
	
	public static void main(String[] args) throws FindFailed {
		ImagePath.setBundlePath("./images/");
		Screen s = new Screen();
		
		// cycle through all tabs in base GUI
		setState(s, "main");
		setState(s, "settings");
		setState(s, "recipe");
		setState(s, "hlt");
		setState(s, "mlt");
		setState(s, "boil");
		setState(s, "chill");
		setState(s, "clean");
		setState(s, "timers");
		setState(s, "logs");
		setState(s, "checklist");
		
		// cycle through checklist options and confirm behavior
		double similarity = 0.8;
		//Pattern checklistTab_startButton = new Pattern("checklistTab_startButton.png").similar(similarity);
		clickCheckbox(s, new Pattern("checklistTab_startButton.png").similar(similarity), 0, 0);
		clickCheckbox(s, new Pattern("checklistTab_fillHLT.png").similar(similarity), -26, 0);
		clickCheckbox(s, new Pattern("checklistTab_heatStrikeWater.png").similar(similarity), -62, 0);
		clickCheckbox(s, new Pattern("checklistTab_transferStrikeWater.png").similar(similarity), -104, 0);
		clickCheckbox(s, new Pattern("checklistTab_doughIn.png").similar(similarity), -32, 0);
		clickCheckbox(s, new Pattern("checklistTab_mash.png").similar(similarity), -20, 0);
		clickCheckbox(s, new Pattern("checklistTab_mashOut.png").similar(similarity), -33, 0);
		clickCheckbox(s, new Pattern("checklistTab_batchSparge1.png").similar(similarity), -55, 0);
		clickCheckbox(s, new Pattern("checklistTab_batchSparge1_vorlauf.png").similar(similarity), -58, 25);
		clickCheckbox(s, new Pattern("checklistTab_batchSparge2.png").similar(similarity), -56, 0);
		clickCheckbox(s, new Pattern("checklistTab_batchSparge2_vorlauf.png").similar(similarity), -58, 25);
		clickCheckbox(s, new Pattern("checklistTab_boil.png").similar(similarity), -17, 0);
		clickCheckbox(s, new Pattern("checklistTab_whirlpool.png").similar(similarity), -37, 0);
		clickCheckbox(s, new Pattern("checklistTab_chill.png").similar(similarity), -19, 0);
		clickCheckbox(s, new Pattern("checklistTab_clean.png").similar(similarity), -23, 0);

	}

}
