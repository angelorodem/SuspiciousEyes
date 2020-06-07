
# Suspicious Eyes Image DLP Example
This project is a very simple demonstration of image DLP System, it works for text content in images and for image identifiers.


## Text in images
The first feature uses [Tesseract](https://github.com/tesseract-ocr/tesseract) [OCR](https://en.wikipedia.org/wiki/Optical_character_recognition) to locate words in the image, with a simple algorithm we construct sentences with the found words and apply Perl compliant Regular expressions to search for matches with the DLP rules.

### Example 1
![Text recognition](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result1/result1.png)    

> Assembled text: " AM Android is the world's most popular
> mobile platform. With Android you can use all the Google apps you
> know and love, plus there are more than 600,000 apps and games
> available on Google Play to keep you entertained, alongside millions
> of songs and books, and thousands of movies. Android devices are
> already smart, and will only get smarter, with new features you won't
> find on any other platform, letting you focus on what's important and
> putting you in control of your mobile experience."   

![DLP Rule](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result1/result1_rule.png)
![Detected rule](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result1/result1_warning.png)    

### Example 2
![Text image](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result2/Result1_screenshot_19.05.2020.png)     

> Assembled text: " A . Teste <Hey, im leaking this secret info
> about Joshep jhonson, his secret id is abcd-1a2b3-uu77"

![Rule](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result2/Screenshot%20from%202020-05-19%2023-41-31.png)
![Detection](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result2/Screenshot%20from%202020-05-19%2023-40-38.png)       
Note: there is a limitation with dark on dark text
## Images in images
This feature allows to search an image inside other images, this can be used to find marked documents with special markers or just by some common item.  
In the result below you will see examples of detection of distorted, skewed and rotated markers, this was all made using an algorithm called [SURF](https://en.wikipedia.org/wiki/Speeded_up_robust_features).
![A](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/a.png)
![B](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/b.png)
![C](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/c.png)
![D](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/d.png)
![E](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/e.png)
![F](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/f.png)
![G](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/g.png)
![H](https://github.com/angelorodem/SuspiciousEyes/blob/master/images/results/result3/detections/h.png)
