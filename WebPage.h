// Based on Virtual joystick by Alexandra Etienne and Jerome Etienne  
// http://learningthreejs.com/blog/2011/12/26/let-s-make-a-3d-game-virtual-joystick/ 


const char HTML_text[] PROGMEM = R"=====( <!doctype html>
<html>
  <head>
    <title>FH@Tbot</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">    
    <style>
    body{
      position:fixed; top: 0; right: 0;
      padding   : 0;
      margin    : 0;
      background-color: #333;
      width: 195px;
      height: 133px;
      background-image: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAMMAAACFCAYAAADmfkzbAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAA2RpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdpbj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+IDx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBDb3JlIDUuMC1jMDYwIDYxLjEzNDc3NywgMjAxMC8wMi8xMi0xNzozMjowMCAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9tbS8iIHhtbG5zOnN0UmVmPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5cGUvUmVzb3VyY2VSZWYjIiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHhtcE1NOk9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDpGODdGMTE3NDA3MjA2ODExODhDNkIxMUExNUEwQTk0MiIgeG1wTU06RG9jdW1lbnRJRD0ieG1wLmRpZDo0NzA2ODJENjExQTUxMUUwOThGOUI2RTJCQjhCRTI0MCIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDo0NzA2ODJENTExQTUxMUUwOThGOUI2RTJCQjhCRTI0MCIgeG1wOkNyZWF0b3JUb29sPSJBZG9iZSBJbkRlc2lnbiBDUzUgKDcuMC4zKSI+IDx4bXBNTTpEZXJpdmVkRnJvbSBzdFJlZjppbnN0YW5jZUlEPSJ1dWlkOjEzZTdhMjU0LWJmNDQtZTU0NC04YzU4LTJmZTdlNzE5NTlhNyIgc3RSZWY6ZG9jdW1lbnRJRD0ieG1wLmRpZDpGODdGMTE3NDA3MjA2ODExODhDNkIxMUExNUEwQTk0MiIvPiA8L3JkZjpEZXNjcmlwdGlvbj4gPC9yZGY6UkRGPiA8L3g6eG1wbWV0YT4gPD94cGFja2V0IGVuZD0iciI/PoDMIuwAADbMSURBVHja7F0HYFTVEp0ku+mNFBICSAdBPkoHQQRFekeQHnrvvXekIygdBEV67yC9SpdelKp06SSkJ/vnTPbFTUh5m00g4Lv/rxt2375y75ypd2asDAYDaUMb2iCy1qZAG9rQwKANbWhg0IY2NDBoQxsaGLShDQ0M2tCGBgZtaEMDgza0oYFBG9rQwKANbWhg0IY2NDBoQxsaGLShDQ0M2tBGqg9dQl+8fPmSnj17RtbWGl608W4PpCn4+PiQnZ1d8sAwd+5c6tOnD9nb22uz+d8dVgo9mfldmgJCaGgo7d27l8qWLZs8MERERMh7SEiI8uBWxgc3vCeL/D49T0rMR3zzkNjcGJIBoLc2oqKikq8mxUM4Ud4enrUd7O0rMdpCjE9r9Q6ttiwOq30Oz1++3PIi4OUGE5vJ8B8kftPnNlhZWdnwSGer1/vqdLoMeOfPPXU2Nm56nR76haNxvsINZAgJDw8PjoyKes5neBQSGno/MiryAX/2kD97aYidPmn1rsyxWjBgEqL0el0eVpsqM8oC8byQGgZGnB2rUjyRr/0wPCyMovg4W1tb4smOJXUi+aXnz01tksjISIoIDycbnY54QWKJujA+F47V6/WxroHP+QA5l+k1lOtARPKCyj0aweCi09lcM3EgRP1HAGFKlAb+hxXPpS+v5/8c7R0K2trq8+lsdJl5ftx4Hh2Mc2NlnP+4EtTKwd7h3/MZKJL/84qB8DQiIvwWg+MC08bp4JCQSxGRkc/fFWDoVEygAgarqCgD0zY/dGRkEMROyU8/JVdXVzp29Ci9ePFC7AvMm6Kn5cyZkxydHOnypcvyGQg5KCiIMmTIQBkzZqSrf/5JAYGB8jsAy9nZmfL/73909+5denD/Pjk6OhJzGwEBPg8ICKCb169HEzYAwt/lzp2bdHzePy5fJis+DqAEKIKDg+U6BQsVonv37tHZM2cUUFrzTyNNwPC+q0qxCJA5vo+zk1NpRwfHcna2tv/jufXij214faAXR/A71jhYjS4e5xp6G2trP52t/Qf2dvblyNUtjIFwj+ng98BXgbteBQefYLoJSMugUCsZFLZvA0mAiRg/aSJ906CBfPjHlSvUopm/EDEIHty6V+9e1LVHDyHkfWy8dO7QkZ48eUI1a9Wiyd9NIScm/Bs3blCbFi3p4sWLVKBAAZr743zKkjUrBTJAenTtRls2byYvT0+aMWc2lfn8c+H0U6d8R9O++07OO2jIEGrbvp0Q/9bNW6hHt25yDK5fqHAh+vGnn8jb21vucdKEifzbKQIw43Prjc8V9R4DgIxSwJqJv4Cri0stRweHsjx3voboEcZMLTgFrmcwCpAoZoXhRhK3srayyuDo6FDbydGxRmRE5K3AoFfbXgYGbGbJ8VdaBIW1yonFcTp+UGtw8OIlSsQAASPPhx9SqzatRRrg+08KFqSOXbpQt85dqFL5r6jAxwWoUZPGZG9nR8NGjKBlS5dRUebYTx4/pgGDB4lqhPcHDx5SsUKFacXy5TRyzGhxhTXx96cP835IX5YtR31796Zu3btT7jx5qGjRotSydStq1bwF1axWncFShup8XVckD8DRlY9TgIDRoWNHkVQMFIPyPKaqwHsGhBiDmAmxmJ9vhml+vr4/sURoyB+lYwC8YsINgiRITUeO2BdRhld8vVBGxgdurq5dMmXwW5ohvc9gpoXsJlI5TayDtcqJtRYuaiBr2ADu7m6vHejBHJw5gVjtIEJw5507dohL69rVa+Tnl5FVJieWnq60Y/t2OsVqy2+HfxNVxoG5Nb4/fPAgnTp7hnb+uoM8PDzIiY/PmjUL/fnHn3TowAHatnUrhYaFkidfy9vHh148f0G7du6kvbt3099/3xbVK4rtDjb45Pemw8HRQdQwQ7RXwdooFaxNnvF9AIGi9kWxBMif0ddvqp+P7zz+u3xU9AhMZQAkBowwo63pyKBsyqBYwqAYwGpbBhPp/FZBoVPxEKYTbWVna2c4eeIk/f3XX/RBliwxB0JNYZYsevnvp07RU1aJVqxeRbdu3mK9vSB9N3kyPWZJcOjAQZo+exbVrF2bGjZuRBPGjZMA38b166lHn96Unom8YuVKTOB76NGjR7Rxw0ZatGQxLedz5WDOjs8uXLjAHM9J1LXV69aK+pU9Rzbq33eH2BPBIcFyP4UKF465v5MnTtAff/whhjYbFFb0fkXflWeJ0ul0Hp7u6Vq5OjvX58VzZgKEBAhPQ/fKdnYUbAd7BkVzBupXz1++mPfs+Ys1bJKGvU0Pn83w4cPj/eLQoUO0e/duaxOVwsrZ0amEA3Oc58+fh/926LBw27t37tB4JujtW7YRfye6PAzdPXt2U6nSpemTTz6hju070EHm7FB79jAXBwevU7cOjeRrL/xxgejxJ44fp6ePn4gN8Ou27TRowABRd27dvEnnzp5jNawN3bt7jzq1ay+gCgwIpH379lKFihUZCDmobcvWdIIJHsY47JZTJ0+yEf5ADGpcc9TwEQI65kS2QSEhF4JDQo4rTicjZzK849LA4OLsXCaDd/qJvA5fsQSHkyA0Dd+72Cy8xs5ODg5f8j1/FBYefoVtviepISH8Wd3Oli2bxQb0awMEBy7bpWMnIVi8jIZpzPeXL16inxYspAqVKjJ33yDhcBz34vlzmjdnDvn4+tD8ufNEFQLBQr2aOX06lfi0JNsUS4kBJ2oNXKzr162j8hW+ogP79st106VLJ9c5zRJo8aJfKG++vLST1aX06dNHUwhfB+f8mQ1ovGDW2dnbidR6j9xGChAimQHZe3l4tHdzcW3BBGZj5LwpOuCYwBqB0cR1YVuIiPBIgyHCztb2s0y+GT568uzp5GcvXqyNo76+kWXTJfMBhLDwSvB7lgKQHMyJY4CCzxFDgO0AIgcQMLH4HO/wMEHfd+TfgZgV9x1+j2vJ+UDQxs+hEkEa2drayXssSsH5+Pxx7yslF/Itq0V4kEiejww+Xt4j2CD9HIZxaqhEwUFBzLx8ydnFRSS1DVzYOl1KXgJSAvfu7OXhOZo1iHz/PH482ejpsnlTklvbhfduSgMBAgMgH3PTufxexigNUtRNjCAoVF549PYfPkR79u+jWXNmC6OD5zAV6vSGAwAuTs5N+bmmshSCOzDyTRnWGhjeTbdpJNtvhTP6ZpjJKlJ2o5coRQa8gAAApAFUIsR3hg4fRj9M+56++boeFS9Zknr06iUqLFQn2GEIcKrZ+6NWSgDYLB3K+vn4zmTNIvObAoROo693CgQCBOacxX28vafw325qosVq1F4QNAZiOl9VrEjlypUT7527u7t8t3jRIrp89U9q1Lgxte/YQTyB9+/do+PHjtG2rdvEAQIQQaWNb2uOuQOAYCDkZ0B8f+/hg25sXP9tVJkMlEqBUg0M75hEcHJ0LJjey2uiEQghlp4YQVJw9bLlylLnrl0loIqtMAf276clvyymO3fu0LTpP9CPPy2kS5cuUfWaNWhQ/wF07do1+rTUpyI5GjJAHj58KJ7BpYsX07OnT8X+s9Q+gw3E0ulDBsS0uw8fdA4PD79rBIRVatgQmpr0DhnLrDrkYGN5AhOZp6VAgDQAJ8+ZKxetWLWKlq5YIRKgcYOGVKZUaerWpSut5s8PHzpEbVq2EtAUL16cBvbrTwsXLBCwjBk1mqpVrkJfli1L69euZfWpJx347TB9Xb++qFnYV2apYiOA0Ony+vn4TNDpdOmMUiFVVCYNDO+GRDAgmJbBO/23rIJkslQ1gmH86tUratGypRjF4OLVmagBBBA5gIINmPDGubm50aWLF+W7yhUq0tIlS8Rzh+9wDP5GYHXEsOH0abHiDIp1NH3WTJrPkgTHBAcFWy4hDIZAW71tEWYEQ/lc+jhzo4HhP2YnWKX39OrLKsMnRvdpsgeM3oiIcBo/cQKNw+vbsVSjalU6deqUGMXQ+U231QMYCJaC6PVGN3lc4oa7G6CBMT2wXz+qWbWaBFu3bN8mgS5svEwBlSnAycGhsrenZ7s4BrWVBob/BhigH0d4uKdrwLZCTUu9RpIvwmD4fsYM+qZhQ+H2302aJMRsGjRVQAD3KVQpEDP+hqcJEiWAiR4bInG+WAaoTkdubHDDqK7wZXnZOrN24wbKlTuX/M5SQERGRQW7ubi2cnF2LkfROwdSVF3SDOi0ayfgFeFgb5/Pw929EwMhzBKjUfEYfTtuHFWpWoVqVatBJ46fIPd07q8fx6qNjc5GckhgIGPXsZeXlxAzQHD1z6v02+HDdOzIEXrCxnJcDxKCcygmUat6DVq5ZrXYI1DDsIcsoUCtWjzw/dl5pfPoExIaegWZdWSSfKZJhvdYPeJh5+Xh2Z3/huFoUWQZ3L1Js6ay7b1poyZ09epV8suUMVbgDAYviP2rihVExdm8bSs1aNRQjkFyFDY73r9/n4oWK0qz582lg0ePUO8+fUSNwu9MJRB2LmObvX+TphT4MoCl0fTo3GEL4xF8L6EsgXJ4pkvXIQ4NWywhNMmQNsEgucburq517O3sPjPuPE32gIrz4Ycf0qjRo2n40KF04fx5OnriOI0aMZJWMteGrQBvETg83Ki1atemjes30KABA+VYU0JXtuJgs2WNWrWoS7eu4lrt2K4dHWP1yIWlAlQxT5Ykq9atpT69elGjBg3o8LGj1LpdWwnewfC21MPk4uRc85VT0O6AV4H7jXRs8ZYNTTKkUe+RXqfzc3dzx8a7cIsW2RDNqQcNHSJxgtkzZ0lG4Z27d2VzIwxjAAE5IutYv/+0VCn6msHQqkUL2YoPYxoErrxAyJAEDx48kMzB0iVK0tmzZ2j95k1UuUplsTEAlit8rfFjx9LEyZPl+CEDB1EvliI5c+WMzlu3bCA1VeeRzr0tctpTypjWwJD2gADlO9LN1fVrnY1NVmxztuSkyO1AcOxLNmj79enLhnMDKsLqS/fOXWQHsXiLmHgBEOw5qlS+Ah06cEi8QwBKfEYvPoNnCcdgd3FL/+YCsvkLF0pePCSJPf921oyZEqeYytIGO5Hhgu3YqROFhVq+sxzuZVu9bUFXZ5fK/M+wlDCmNTCkPcPZwIT2AS9yLaRLWsxCWUdv16GD5H5cv3aNBgwaRNO//17yzhWp0KtvX8qTJw81ZnXmwYP75OLqovr8iicKuSmbNm6kqXxuZBniupAqiD/AzVruiy9o3LffUu26dSlbjuzRATnLARHJqmQjvo4nxXZFa2B4T2yFSDcXl5o2NjZ+lhrNILgcOXJIktWMH6ZTHSZEcPSfF/4kAbFXbFRnzJSJGjdtQr179qIb12/I5+buRoUnCaoQotO2drZUr359yXTEZwjYrV29mnr27iUJXvAyVa9eQylOZ7Exzc+TmxnHl8a5stHA8P4AAb56XxdnlypYaEtPCnXl83Jl6enTp5L516y5P21Yt072Hkkpn1y5qHqNGnRw/wG6dvWqEHXc2IHaAaMZdbIQgc730Uf0ZfnyMZ6jJYuXUP78+Slz5sy0euVKqlK9mkiNlNgCjoocqPrBqpuLpdJBA0PaAkOEs6NjGbYVPjAazskeMFKREdiocRPy9fVlbl2PPvjgA8kpBwn2HziA9h8+zCpUe8r7UT7axWrUUFZ1lIw2s0D36hV9VqYMHT15QmySTwoVpNXr19GqtWsoS9YsAkSUBYKbtkiRoka1qRwFvAyw2CEKpmGn1+d3dHAsQhZu9dbAkHYMZ7wcXZycvzJYyDKhHllZWdPc+fPJ2dmJenTpSj4+vhTIRAvCRNmcbj16sGrUgz4r+SlV+OJL6sqGbfuO7alT584x27nVgi5vvnyyq3Xnjp305edl5Xx4OTu70Oy580TaHGbgIRdi7pw5LJ3W0+x586hQkcISg7BUOPDD6l2dnSvQv8HKZEkHDQxpRyoY7O3scrLO/T9LVCTFMJ09bw59XPATatu6DS1cuJA8vTzp9t9/iyrUoXMnMWYXzPsxutQnEytUmeFDh1HLNq3Jy9tbtboEMLRo1VKM814MMESZ4W06f/48tWrenFWmfGI8nzx+gvwyZBDvUrcuXWj/vv20eu1aliKFxB1ryVYNQ1RUqIO9fVFWMf1M5lSTDO+oVJDtBE4OjiWsrcRvHpVsILBQQXT4Y1ZFxo4ZQ7PmzqGrN65Ty9at6eKFi1I0gXVsKcWDuAHAgRfiByi4gGIOmdioVqqwJ6GiiEGeN99HtG/fPrEZYDQrsYmbN2/Sn3/8wapREYl4Z/DzoyPHj9HJM6fpBoMHNa+gShUCIF6+TDYgWIxG8DP4ODk4fEIWbPHWItBpRzLYOTg4FIVBaIlEmMnEX6xYMakyiMAY4gAorAZ7ICjoFQUGBohN4OXtxb8Jk6ohCofHZyDIQObUapvURG8HDyRfVsNMbQ18jviFO9stkBZhxmqL06ZOo8ePH9GVy1fozytXaMHPP0t9rfpf16Mzp08LiJKpJdqw3VDiRUDAduOcRpKZSUCaZEgbYDDodXpfW70+Z3KCbAoXnz5zptSR6tC+PT1iggNhgdsjLRM1p1BF5M6du7JBr0+/fpLHALDghVhBvwEDhSD/uvVXrCroCd64sbIJbIDadetQyVKlxKWKrdx4tWzThry9vKTWLiQOAIfqh7vYtkA2HArGDeD7gEoFQPzPWFw6ORICDgcGXz4Gcbrk0rZOI8S0YS/Y2dnmZFHvYdydqnoo27InTZkiKZngvjMYFHbGiuggwr179lBQcJDsUIVKNGzIEFrJ6gkSe5YsXiznaNK0mQTb6tWuIzXm1RIkQISMuM8+L0MbNm+SWlmwH8p+UU6AObB/f6l19QXbDa8CX5F/yxbkz7YEXLu4RrSxbyXR7IW/LKImDRuyxPhDgGqOhEAV8ei+EraZQ0JDnlGcmrMaGBIekOchxsmKMuXQb8leMNjb2uUxrodq4xlqCYh/wuRJVLV6NSakRlLMWVF9hD2yugPdvTMbrSgQrWSmIQGnS7du1ICJD56n/SgDM2Mm3b19R+pTqSVEEDKu0b1LVzGSISG+KP+llB9t9M03LBX2iR2RMXMmvt9Imj9nHm3dvDkW2PAcQUHB4u5duXo11eB7u337tgDNDEBE8TkdWTrkYDCcTY7d8J9Tk5A77OLsXNrb07OHk6NTaeYmnhR7x+ObLESsXEtva2ubncywF0Ak2JY9cPAgqVjRu3sPKfUZxaeAa1R5IanGzdVNuH/WbNlkhyqixP/884+4U3v37EkvA17SiKHDxdtkDhBilHWWNgAE2gUc+e032fFavUpV8RgJQTOx52MjOywsnDw8PWLdH16QEtbWVpJTfevWLVq6YrlsHExGlNqaJWyO5LpYdf9BMETqWZy6u7r586sRE8nDkNDQE8xNzgWHhPzOC3DNEFtaUCpKDCVvwQki3kCkOvwL3bp12zbC3ZGxhgDbjj27hfjjJVhrGyH0bNmyyxYJSA8Ytzdu3GTj14dBNZD19/6vZbyplQ6Idn/JEqETS6CunTrLtg5Ioegt33b08ScfU7bs2Wj5qpXxqmC47o7tv1KzRk1ow5ZNErdoUK++qIA6tdX7mJmg+xD/hQyiCHOZ2n9STYJ+aexUA0L0dnJ0REONOvzZy9Cw0LOBQUGbmdj2RER3molV3DcV7AVwVledjU06VZ4kJiS4IStWqkgjx4wR3/66tWullu3RI0fi5erKZ3BjVqlWhU7//ruAAUT28P59GsiG848LF/Dnp6U3hlLLVt1DWEkEGq0FUF1904aNtHbNmhhQwWjOnSe3VGJHos9JY3HohAD14OEDqlenLm3fuYPtoMnUqUNHkTpqvFtgJnq93suamUsUr6UGBrNd1BTBOqviVNfZ2dqVsrezL81S49rLgIAlz1++2MTS4xWlYHphXAOeF9oV1ajVnBslWLDDdMbs2XT29GlWbW5TfdbNTb1Kr+kOTEioa7Ru7TrZEjFz+gwhUqg3MFS3bNxIkydOpJlzZgtRAhDg7HH758UHMqhYWbNlpWUrV8j2iv59+wrIFO4PFQjXvHnjFt25c1vaBCSkhuE+8Vu4dpcuXkK9+/WVHIxp301VmxAEu8GN0JPuXzCotgW1OEOcyURHGymQbG2d1cPdfZirs3ODFwEBS5+/eL4+KtrtmVJSIkafZbUNYNAntQ0Dej9Uj8lTvxPuimLAs4wEnNgAUd9kdQiRX6R9AjyzZ80SDw4GADFx/AQ5zwzm7sVLlqBJ48fT/fsPJFagj6dBJYgc27frfVNfqmxc+/MatWjWTFyqinoEwGXNmlVsmv59+1Gt2nWoTbu2IkmSUrtYdZWCAr0ZXOfPnaf9e1HSxikpcBp43ex5Pl1Cw8IevhOSwcqI1bhVsSFy8f/IyKjXngLfRYRHxDtxCB6lYK1PZWJD8WLuiXzbES5OTpUeP3s6/VVQ0O8poDpZmb7zszmj9axRdUtwQI0YOWqUlF8pUbQYPXzwIEnurcwRWoU9evyYpk6eQj379KatW7fSI5YWSOhRKpZPGDtOotRjJ4yn2nXq0Iply8Xzc/3GdXGLRldXt6WsvlkliaepfzP6MG9emjt7jmzvgJtUAYJSXaP/wIF07dp1Wr5smXR2ggRS6yAIYeN60nff0dTvp9FXX3wpQEviedHCV2/sVmq2M+StgCE8HG1v9SISTYkYnhCQlouL82t7YyIjI0Qcg9uYGnnwRuT/XwHhlFgMNcSRHFDY6vUl/Hx8P2HVafGjp09mpUC5dCsTnNsm5dkDEJCSib1DfXv1pjJlykjFCrVMQAmQIQMNEmHU6FHUplVrmWelQyqqWmzdskX2D2H3KXa8NmnaRLg08hDQrtiF1RWlRhLalHVq30E6KWFN4EJVhBu+R14D+uyhv0b7Dh3ke3OZ1ipWv/DcfQcMoD49eyYJBtC0CRgoTUsGAAAuPDwURCgCMkr5EEzkjl9/pcZNm/L7jhjJgUmcP28+zWcjD3m5cNkhugouhI6gpT/7TFpdtWvdRsChtOBNYf0piE9q4+ri0p7v98N/Hj8awaJYqf1pSKaUUMW5lD1AqIaN9lx7du+iRaxTQ70xL//ASnz9mEMUFx44eDCNGDZM3K2KgQoJAY4+Z+YsCaBhjZD34JvBV9YOoIDKhfwH5Elg7bAWpgNAKFW6lMQ/rly+LB4suFbVr4lBJhIAQ4wEjS0RoUYuBAzwuL04EphTqzQvGTDpmNDrLDqLFismoXgFDHjINatXU916X1PVatWk4w8WB99fZO4zdNBgGs9iFtUW/v7rbzkekgL+8tHjxtJyniwkssNYTE7GloqBfmQv7dFlJoPfgn8ePx4e8CrwCP3bGtgcN6zqhYJUgNqCrDXkG9+8eYvKff55sje2gWF8Xa+eMBCM0SNHCtiUdYCkcDbuEQIxYpOdcHSD9LOVJvP6eECA49EP/POyZemXpUtEIkByKOtuTtzCtIkN9jXt3rVLsuXgRlbZdMYqjjpqSHNgUCbm6NEjsrV34Y8/xtH/w2nWzJnS4ha7GpVcWkz8r9u3i6969py5VK9uXQnKYAGhi/bv05eNrT60fPVKatOiFf3555+WbPpKSkq84nvN6OPt/QNzy7HPXjxfQ7EjnoaUkg5KeUdsk4YthWc07V5kbjwAKmoEq5y//LyI/Js0oYU//yw2SB/mvo//ecQg+Ld6tpWxYSVeSbWxwlrgmFatW0uP8F/4vD8tXEjzFywQSQ1bQ+09QyLgOtFEby3nhesWajIy5pC/jTlRoYKmbTUJAw/y+8lT1LRpM9lnj1C8wjkwEbt37qLGvFBwyaHvm8KB4F6bO2s25cyZS+r7oAe0RD/ByVjUTxg3nh4/eswcbwl1YWlx7OhRi2v0JEKkCI/qvD08Ruh1Op9/njyeHWchUgyFIIwjvx0Riefp5Z3s88CIzpI1PRUrXlzmf97cufTkyVOpjHHg8CEp54KkfkhbpVGkIkmQK425hHqkqLvKdhAQKzbZYWcs0ky/HT2GJk2YQJ+x+gopf+bMGTp/9lxSBBwDQJTBxz2YghJFzCAhnz1/nlT/B2Aoit6VLdx4mL/ZbkDmVb6P8kupQtNADL7/Ydo0UYkQ2se2A0V0oh/c4IEDRcQPZX13COu9WCRls9cCljTYMgyf+eABA8UgVFyIqTAiWG8yuLu6duUbcGM7YoIRBOYCwiopwxfeGkulHOZx0eJf6Hcm5A3r10s6KOyH8mXLyS5WuFW7du/Gc7hA1BI0IwEQIKX7sQGLQB06p7Zv01Z2umLLBKrrNfX3p2rVq8uO12qVKtPx48clcIeiYsuXLpPoM4qJwQBX08hEvFEJfAf1VweQxj8XmEckdYS9M2DAAsNIu3jhvLjo9u/bFwsM+PvUiVN07uxZat2mDY0ZPTqGw0tnUDYaO3foSCvXrJEdkBDHCsHjHbYGADH1h+/F47KIRbYCmFQYrL1EBbm7uPjDMfzw8aNxcRYiRSREcrZJmA5s00ClPPTfrlurtsw/zokXOG7/Pn1o0U8/UZt27WjI0KE0fMRwOnfuHF26eInqN/iGxo0dSyeOH6M58+ZJyubLly+oVKlS5OHhSacZBK1btBQ1FtJCWSuoV5AQu/buEX1/0oSJYoukgFhO2gJPjvr+tqJbrFrQkSNHpKitbTzuMnvmQihvgrqfOXPmjFVnB75xcKZO7dtLeUMYbKZlz914MbBhrHmTptJyqSdzJezlSelYhKkZAUC4MiDSe3l1oVQqmZ58tEYK0cMbtWrlSpHEpuCCygOXKbrx9OjWjUqXLMlqZmdRiRCkw4AnZ9uOHcL1y39VnvwyZqQpkyZTebb7ateoIfEI6chq4unBv6EBTBg/QfZQYddsaGjabU399sDAEwU9EvYAtvfG3UqA77E4SDLH5i/THYwQo1jMy5cvi8qEPSxojK4ksoMt4Lxw2zas/w1VqVaVxrCaERIckuxSKGoAgRqgrDK19XB3b0ixS6a/VUBAKjRv0UIi1pOYMO0T0N2h02Pe4BpdtWKFbL9GrSMQ9OJlS2nl8hWSo4C9UDevX6fZM2fSzRs3BACOxjbGcQdsOeRMIP0TW7TVpJOmhPKRHOnw1sAAdQfhdqQmFipUON76m9AP57ORV4B1zsKFC78GCCwcsrig42JbAo5XJAi+x79x/m/q1ac8efPS9NkzRTqkRDW3BCVEZFS4h3u63i5OzqUoFXoImDswr/AWde/Zg76bPIX++usvYTRJqbHIQuvQqSNz/0nUspm/GLWZP8gshcI6tWtP2bJnp1p16iRJ3HCMIDd6xNBhVLlKFfqqQoUEd9ampH/jnZIMCsGeOnlKmurFx7EBGOy7X8FGWBc27KLiOQb6KQJE2HE59YcfhNAVdUjJ9MJemGaNG8v5fmYD0t7YxziVbAhQh523p+dgvV6fkSjGs/FWAIHnhDv2HhvDi9i2AqdWI0lQfQ/Fxo4eOSq/RV+32jVqSnW8KJ7XxYt+oVZtWsdqXp+YvYNSMSuXL6dhI0fIPaSihH731CRFp4T7M3fu3AnGBMDdUbQW9f6/KF8+Vnl0hYs5OjnSkEGDxM4YzMZfYEDga9fB6Ni2nZRQXLZqpZwvJbrJJOR2ZSLJ5u3h2Y0srOVjycBclf6stNQ3HTliBAUFBycZ/AIjQbtbuLXnzZ0nxI75AwFLjgITNt7hgsW5wOmDg5KumA+mhM2AqMPaqm2bNyEd3i0wwI8NIw1RzVy5csWrvmDCsajz5syljp07seGtfw00itu1c8dOUqnNv4W/REJjuc3YSEReMBLQd+3aJYDANQMtrNmTCFG9cnJ0rOLm4lo+2t33ZtUlJUA2ZNgw+nXbdtqza7cQsRqpgJ2okAZgVAnlHkD1QcAUCUZ6FUFA3AtSOSeMG0ddunaVHIe0Zky/VTBgUuHlQVukosWLJVi3H2IWe5DwPTZ+xcdVxMP09KmE/7t07y7Bn7iN9QAscLiJ48bTj8z1EKsoWrz4a8BJOQFhIHc3t1Z8XTdKgSrRZhnN/Ozg7vDgIEahJlsMAErHUgFbNWCrJRYTgITARj2oO1WqVmVVNGnpgLlftniJGNPYFwV7IzV2CLyTYFC4OtyghYsUSZBDK4GnH77/npq3bCGxhNd0TniYmPMhVD9owACaNHmyGI5xSyXGVGJYsEDKpSPQhB5nqQEIqEu2ev1H7q6uNd6kdwkSFgW7+vXvL4k88Kqpif6CyXzNUgH7vo4diV8qmM5jRESkgAY7WxGQS4qwwYzC+N4w78jUwystqUtvHQzQR0+f/p38ePG8EylrCPfdoQMHZeOYf4vm8U6i4mHavnWbEDsKakGqxOfxACA2b9ok+bojRo+mZv7RqlVKcyrkKLi7ujVD3jXFjk6n2pCocY/uEuGfO3u2KvVIpEK6dGw4fy2JP2okCdZk185drO6EUc1atVQRNtYDzG/5Mjam2Y7BeqUVY/qtgwGTfuf2HUkZxP6W8ERaHAE430+dJi69TJkzJ+jWg4cJnWQQx5j2w/dyzvgCbqgaYRqc69W7txjfKRmcQ1Ewln6ZGRD1jJ4lm9QEBIBQuEhh2SIxZtQoAbiaLRBI3mnarBkzmxuyRcMuEakQV2JjC0wT/i0IXc3cQeJMnjCR0rEx3bZduzQjHdJEqRjYAmfPnqUSn5ak8ET81hD1aLh35PBh6tix42uepVgeJl4YBOQcsJdJPEyvV3vGDhglOIdKDJWqVKYx48YKQaVkcMgQZQh1cXauydIhs1E6pIoxrWxtxvMePnSYNm/cpEoqSHfO9N5Uo1ZNmj9nrlkJUpAOKFL27NlT2eoRpMKzhPMjbgHbrXO3rmkmMp0mwIDJOXb0CEuGArGypRISs9CDS31Wmj7Knz/BSVS4IQxqVHdr2tw/XrtACc5hRygAAS8H7AglhzdFiJQM4Xw/GdxcXCuRhT0EktL5oa4gTwRSAcBQ4ymD8YuOnTBssc9IjX1hyngwsJu4mX8zYS5qpINpZBq1nyLTgDGdJsAA9efC+QsSL8j8wQeJ6pCSKXf7tiSOYF9SYgSLRX3+7Bl1ZEB07dZNGnzH9TApgJDgHHO1Zo2bSI0hBOfA9UJDQlLE9QrbgUFXiY1Ij9TwLEGSwYc/gAnrJ7aXsIPUXoWqI1LBJ71U5JtrplQwlQ6/Hf6NHj54KJ4oNWqPGNPGyDS2eJSvWCHJQgH/CTBgYlAW8e7du7LtIqlukODk6EuGDXxI+UyouYayhwlFs4YOGUKTvpsiVeUSOl4JznVgPRaZeEgU8k6fPkWCc7AdbPX6HI4OjoVSQzoAyGhkCGJGr2W1u1xBgI0aN5I5OoN6SmZIhVhraGMtSVnIX0DQTo10UCLTKDyAXAjnt2xMp5nykhCQyG+VrRlJTCRUIOxaRY5u565dondlJSBiFQ8Tcod/XriQZifiYVIkDzgqCuaiwhvqASE4F2B5cA43qHd2dCxN/7a4TRHpAOmFPmpoSTX+27GyhUWNN0gyyPz8qEbNWpJEpcZoTswoRpDu7p271KBRI9VGMX43aeJEyY1AGZm3aUynGTCAKx8/dpxyMLeH2zMpzgLpgHxpTGbV6tWTNNzgYUJhXezRR3vWsLCEy8sowTnsv/9x3nzJnANILY1FQDrw/RZiMHspKndKGM0RzE2hdyNPHBWxnVUYzYo0gZsaeSMXTXLRLfEMzp83T/Y1QWVTw+XFmFYi06zK5s6T560Z02kGDJiUG9evy2Y87FVKamdpTL40G9PgKA4Oibv1YjxMAwZKcG7QkMHC7RM7HqCE/j1i6FCaPmumFCmwBBCwG/Q2uswO9vb5yYIOM3HVnAqVWOf+6isaOXyEcHsrFcn3OC5jxoyiryP90xKpYMrlTxw/TteuXRXpoCYqrRjTqKD3x5UrbzUynWbAIB1j2Li9whMCLhyuwpMD4t65cyc9ZnsDem9SIlbxMHXp2FFas6II1sskiNsVwbnNm6lLh440fOQIyayzIDhnQI0kR3uHYibzn2wwgPOiXMwQNkJREhK1VlXbCiwVEFfArmHYC5ZKBVOmhl3Edb+uK/Vf1UgH2ebNjG24GNMVqGLFtxOZTlMl6UGsqJpRsHAhSfJX+xsE4hoyGLxVNOaDgYiUUHiYevTsKS1bX8XjYYqrYh1lfRiFc6GX9+rTO9mZc5AOzEHzG2urWluyBiCYFi1bSowAQSx7O3XcHZwXfdvQRwHdN5NrNCc0v4gZXb50iZr4N1VN1BKZFmN6GQ0ZMfytRKbTFBjAnc78flo4SnqVXCU6X/qkVGBQY4ApcQXo14MHDZLyhVkS8TApA4uD8jOIRVSsVIm+ZR0XyUaRZgbnRFXS6TIxB81AlLwqDhiwebJnzy4J/FMmTpJsNFQpVDOw5Rq5CKjGfdWkiFtKAmKWsaUWSryoDWCi2t8kfhZ4o9q2b//GpUOaAgMMsHv37tKL58/p448LqA56Sb709z8IkcbNl07Kw4QkeGTJwVee2KIpIPrn4UNJJc2VO5dU4EhGcC4KVbeN0WhKrkcpLDSM+vTvR7dv36FfFi1SlbSDES7lYrJKzSo4B+wdHFKFqV08f4HOnztHzVu1VBWVVlSse3fuSMkfxJA+fMOR6TTXuQdFrs6cPiMNtNVyFCVfGq5Q5EurbeoN9WfmjBkiJZLyMCmAgKGJ8yM4hyHBOUcHczLnpB4oE4xfnDVQbYSAuNBDrWat2lINL1hF0k6MVOBj0QYXxRgg6VK6Nm2MdOB5AtiQ/JPYPrLXvIQMamQ2ijE95M0a02kODFIwixcKldOUis6qJtHRSbwiqNFTtGhRVRxF8TAN7D9AmvvBPalGNCtV5ZALLBWmV66S/srmtF2ytrKG/9PGXCMaYIUaMmTYUNq2ZbP0c3ZS6UpFETGoVijki3wOh1SQCqbS4fKly3T86DFxOqhlUNHbvP+NTFeqXFm1ZHnvwBA9iZekL0CWrFlUcxQbXXS+NIpWte/YUTWIYvYwdehAZct9IdsJ1CycaeYciiUvXPSz2Dpq79fa2ipZ2zJwb/UbNJCqguO+HUs6vfrSV2AQ2F168MB+kaSpJRX+tQHsJd8B7a1QJ1bt3CiRaZSxhwtc7X6n9w4M4LjgzgGBgeTm5i7N8VRPPhMnCpKl80gn3FLtBILTPrj/QCLU4JpqvRhKDdgJY8dKrwREgdVU3kC7Klu9LepE2poLBoC8SNEiTCyH6CqrOWZtquP7zZIliwQ3UxsIigRFw0JU5MhuBhgUZoNtGoiQIzr9JjxLabLbZ8xuS3B3M7dAqKnWEB+BYfIjoyLN5kC4T73eVnoYmLFdI8ooGWyTYzwbogyoWqba/Ww68Iw2yfhdskZ0B6SY5iXmzquiFr6pzPH3r/WtBRPHenyyr2lOyXXjj+zo31L2VsliGGl8KQzGR0vunaZSKZ//EBjenWGgNFJ+UhsaGNKCDNOAoIFBGxooNDBoI8UsHG1oYNCGNjQwvHfGszY0MGhDGxoYtKENDQza0IYGBm1oQwODNrShgUEb2tDAoA1taGDQhjY0MGhDGxoYtKENDQza0Mb7PHTJ/SHS+EKCg6VBNhLjdXFSCfF9ZEQkRUZGSYqiOQP1VpHul5wyIWi6h+shZ9acTClcLzwiuqG6uddEITE8a3wpozgXEvFxDDqSqsk9Rh41yvLbGCuCx71PeRmiZO7Nm5sImZc3kVyPgflHmilekv9s7rzyvWL+EqIfnBMVyJH+inmyNDMuWZJBmVDU7qleo4b0RjOtG4QHwIJ6enmSm7ub1O5XkwyuFBMG0bi5u5OXt5dZRaRQqsXT04Pc0rlLcSy1tYyUZPOMGTPFdK9XQzA4N66J3mQo8YjKHqb3q8wDOpmiD52vr68cn9g9ofpF5syZpZF5gY8/ltxqBZwKcTi7OJOjg6MUS1CTKI/r4b5cXF2lpI2DcW5SGwioQwXwo0KelxevJf9bLaOR+3VxkYIHeOFcyrzJ8/A84pxonVXCWGPLUpCbDQalWsXEKZNp+apVNPfH+bRm/VqpuoAqb0qFuWHDh9Pob7+VIlKr162NIbLEBn7nmyEDrdu4kYowAc1fuFDKvqgp3YJjGjZqJPeERokbN2+S8vZJLTomEESMUi+9+vaRPtNoaqJGQuCaRYsVpUNHj0ix5CnTpkqdH+V+wd1Rl3UD38usuXNoy6/b5fiEngeSFgWRt+/cIdX6tvLxaAAIAOF+QASo/letenUqU/ZzmsF/S5PBKEOSTAJ1qPYdPEAlS5WS56vFRKS2llFyhsIM16xfJw1lpk3/QeZCDQhxX7Xr1Kb5C34UICxdsYIKFCgQU5cK7/n4eTZt3UKz582VawwbMdzigmNmgwE3UvLTT6W+kDJy5solVdpCw0LlYQsWKkRtO7Snju3aU/my5Shv3rzUrLl/ksWg8Nuu3brKBJQoUpS+HTWa+vbvJ4WvEivBgklAbdYhw4fRnFmzqMgnBenpk6fUf8CAJCVScFCwcBe0uKpRuQo1adhIQIV/hyRCLJh0VJlAx5lTJ09Swf8VkHKVQ3lRwHkxT7ly55aaocoAJ+vQqZNygteYjI65aJfu3aSDjTI6dekspeNR6BhAq1y1KtWpWZPq1qotJfLRlDEoOCjR+8RrwKBB9NetW1S4wMdS/n3EqJHCsVOrBAuevy2vP6Rh6RIlaWC//tStRw8pGZlYOU7cD+5r+MiRtOjnRTKvt27dpEFDh8Q8C47pwEwSZWSU4d+ihQDGEolnNhigp6IsY9zhzqoJuBQ4GBYdD7xvzx7p9XXjxg25cTVNszNmykznzp2lE6d/p127dslnOHeiZR/5O1cmIEgf9CU+c+E8nTp1kvwyZUyydAx0b19fH7p75w4dOniQDuzfL12BsIiRSZSahArgw8cdPniIzl26yL/dJzV+oL8q5eId7GNXrcPcwb6Ke0dSc5JtBPSEMB0AllIHFqB48vgJX+eA1IdCKX6/DH5Jqge2tnry9csga4H73Ld3r5TaR22p1CrdCIUmE8//pUuX6OiJ47Rnz265FnpNJ1XCE/MGQOzbu0fWEqX2sR6Yb3wPmvDw9Ij1O6yzs4XFxswGA7g2GlKgOJTp2LJps4AB34NTorrdelYPIOqh+27asFFVtec1q1ZK55ctmzZJCynUQb1y+Uqihic4KgpVHTxwgBb8tJA2bthArdu2pVUrVkY370hER8c97fh1B4Mwk6gxEL2gzP0MisQKdGFBIOnWrVnL0mEYrVu7liZMnkzbtmyVpooABOqFHj9+LNbvtm7ZIrpz3Huy5n/jfFt5Hk3H4UOHpMI2uvGAOQBMmFO8nJ2caeeOHYneJ64THBxCa1auoh69esp9zpw9i/bs2k33799PtRpKMGrX8tyI2rdtG61cvVr65F3g9UxsLXE/D+7dk2edNWeO3C8kyppVq0XaYN7hYNmwbn2s36HIMV6WlNe3Gc5iPr5xiBdh9+7dVkbAwOtk5ezoVIK5VP4XL16Eo0ShnjkZeniNHTNGSiyCg+Fm0XRk/959lOmDzEJso0aMoENMqElVfMYkXbx4kSftGuVnkXf61CkaNniIqAiJ9ShTJNLevag76kx+LIXQ0WfZ0qUyOYmBQSp/370nRJs3Xz569OgRDWL16ub162LIJzp5vHBoqh4QGEB58uSh9evX09RJk6XwGb6Llo57+Z/W9PTpE5o5fTotX7IU3jcbljovXgYE7IJpwXOUxcXZuRTPXdRxZjRPnjwWybKFgTN29BgR/XgOtOf97bfD0uoJc4w+15cvX06SADCvJ06cEEny0UcfCXGOHztW5sz8ek/qBq6JtrYoFZqfbbhLFy/J/T57+jRRMIgNxO+QfCixD1sU/eZ+YvtRWUswP7QjQ6cnnc6Gj90vtVn5+az43FEvAwN+jYiM/IeiS/5DDzT4+/tTtmzZEpdmCYnJcePG0YABAxQgwL9n7ePl3Z3VkQb8myAskHTlNC68aRFb8STwdyFG/Q2gMaf0OTik4hoFh1XEY1LeC0iBYKNdgkV2cHRU5U1CkauQ0JAYXdbO1o4J1k6VCgHjNSjoVYzagzqhCoEp3jHF8AP3d4j+3palw83b9+725Y9fMdcvkyG9T38mznA+nwHnU1QnzCsAqzQ8x7mU+wSjwfyouk+DQeZVORbnfROV9UwbzENlBIGruV+sv2LgKwWi464lnkdRiwAUng9MfPid+/e6M+1dgDkJUxSnY8Zu+OKLL1IkzmAwvnDycH6YcCxEQmqP6NP8nT6ZTTDitmJSu9iK3mi2h4z/p7jwzLmmLJS1lei4Cd0TCDmB3gnhRs4VpcwrXnw+Q9zzKfciJfGTe59MSGqrdafkiFvp25yC0En1nIiHTpjfWIUZ51ShWSKVOec6lSCQtq1Pnj/b+Ozli+MUXSMUsk5p36oNlf4HI9GH8cK9MoLAJig4+MJfd+/0Nc6praKWatNl1rxiLgGEIJbG943qvYHMKL6gRjIYFA7G4u4Rv780qk22GhjMFED/giHU+G6IdoZFvWDVJxDS3mReta0y5mstmNdgIygUqUupAYZw4yLZKHoYpXCX+/8IB4tUVCLj31HGOVWsykiNySQLEMq8Rpi8R6WGZDCNzESagELjXslbNAUUkcbPrOMwG21ekyd1I03AEJnSksFgwtFMwWCtSQWLRHpUHCPPKs6camBI3rwaTJhMVGrZDKYcTVkorWCu5QtniGcutXlNmXk1pAYYFMlgZcLVSFswixcuoaHNa8rNq1l7TRIEAzbXVa9e3dQ3bdGFtJEiQNFGMgaClGg+mdSwelM9drWhjbQ+NENNG9rQwKANbWhg0IY2NDBoQxsaGLShDQ0M2tCG+vF/AQYArw1cZy9G+KoAAAAASUVORK5CYII=);
      width: 100%;
      height: 100%;
      color: white;
      font-size: 20px;
      -webkit-user-select : none;
      -moz-user-select  : none;
      -khtml-user-select : none;
      -ms-user-select : none;
      user-select: none;
      -o-user-select:none;
      //unselectable="on"
      //onselectstart="return false;" 
      //onmousedown="return false;"
      overflow:hidden;
      text-shadow: 2px 2px Black;
    }

    h1 {
        color: red;
        font-size: 40px;
        text-shadow: 2px 2px Black;
    }
    #info{
      position  : absolute;
      top   : 0px;
      width   : 100%;
      padding   : 5px;
    }
    #info a{
      color   : #fff;
      text-decoration : none;
    }
    #info a:hover{
      text-decoration : underline;
    }
    #container{
      width   : 100%;
      height    : 100%;
      overflow  : hidden;
      padding   : 0;
      margin    : 0;
      -webkit-user-select : none;
      -moz-user-select  : none;
      -khtml-user-select : none;
      -ms-user-select : none;
      user-select: none;
      -o-user-select:none;
    }
    </style>
  </head>
  <body draggable="false" ondragstart="return false;" ondrop="return false;">
    <div id="container">
      <h1 align="center"><font face="Helvetica Neue" >FH@Tbot</font></h1>
      <p align="center">
      Touch the screen and move
      -
      works with mouse too 
      <br/>
      <span id="result"></span>
      </p>
    </div>
  <script>
  var VirtualJoystick  = function(opts)
  {
    opts  = opts  || {};
    this._container   = opts.container  || document.body;
    this._strokeStyle = opts.strokeStyle  || 'cyan';
    this._stickEl   = opts.stickElement || this._buildJoystickStick();
    this._baseEl    = opts.baseElement  || this._buildJoystickBase();
    this._mouseSupport  = opts.mouseSupport !== undefined ? opts.mouseSupport : false;
    this._stationaryBase  = opts.stationaryBase || false;
    this._baseX   = this._stickX = opts.baseX || 0
    this._baseY   = this._stickY = opts.baseY || 0
    this._limitStickTravel  = opts.limitStickTravel || false
    this._stickRadius = opts.stickRadius !== undefined ? opts.stickRadius : 100
    this._useCssTransform = opts.useCssTransform !== undefined ? opts.useCssTransform : false

    this._container.style.position  = "relative"

    this._container.appendChild(this._baseEl)
    this._baseEl.style.position = "absolute"
    this._baseEl.style.display  = "none"
    this._container.appendChild(this._stickEl)
    this._stickEl.style.position  = "absolute"
    this._stickEl.style.display = "none"

    this._pressed = false;
    this._touchIdx  = null;

    if(this._stationaryBase === true)
    {
      this._baseEl.style.display  = "";
      this._baseEl.style.left   = (this._baseX - this._baseEl.width /2)+"px";
      this._baseEl.style.top    = (this._baseY - this._baseEl.height/2)+"px";
    }

    this._transform = this._useCssTransform ? this._getTransformProperty() : false;
    this._has3d = this._check3D();

    var __bind  = function(fn, me){ return function(){ return fn.apply(me, arguments); }; };
    this._$onTouchStart = __bind(this._onTouchStart , this);
    this._$onTouchEnd = __bind(this._onTouchEnd , this);
    this._$onTouchMove  = __bind(this._onTouchMove  , this);
    this._container.addEventListener( 'touchstart'  , this._$onTouchStart , false );
    this._container.addEventListener( 'touchend'  , this._$onTouchEnd , false );
    this._container.addEventListener( 'touchmove' , this._$onTouchMove  , false );
    if( this._mouseSupport ){
      this._$onMouseDown  = __bind(this._onMouseDown  , this);
      this._$onMouseUp  = __bind(this._onMouseUp  , this);
      this._$onMouseMove  = __bind(this._onMouseMove  , this);
      this._container.addEventListener( 'mousedown' , this._$onMouseDown  , false );
      this._container.addEventListener( 'mouseup' , this._$onMouseUp  , false );
      this._container.addEventListener( 'mousemove' , this._$onMouseMove  , false );
    }
  }
VirtualJoystick.prototype.destroy = function()
{
  this._container.removeChild(this._baseEl);
  this._container.removeChild(this._stickEl);

  this._container.removeEventListener( 'touchstart' , this._$onTouchStart , false );
  this._container.removeEventListener( 'touchend'   , this._$onTouchEnd , false );
  this._container.removeEventListener( 'touchmove'  , this._$onTouchMove  , false );
  if( this._mouseSupport )
  {
    this._container.removeEventListener( 'mouseup'    , this._$onMouseUp  , false );
    this._container.removeEventListener( 'mousedown'  , this._$onMouseDown  , false );
    this._container.removeEventListener( 'mousemove'  , this._$onMouseMove  , false );
  }
}

/**
 * @returns {Boolean} true if touchscreen is currently available, false otherwise
*/
VirtualJoystick.touchScreenAvailable  = function()
{
  return 'createTouch' in document ? true : false;
}

/**
 * microevents.js - https://github.com/jeromeetienne/microevent.js
*/
;(function(destObj){
  destObj.addEventListener  = function(event, fct){
    if(this._events === undefined)  this._events  = {};
    this._events[event] = this._events[event] || [];
    this._events[event].push(fct);
    return fct;
  };
  destObj.removeEventListener = function(event, fct){
    if(this._events === undefined)  this._events  = {};
    if( event in this._events === false  )  return;
    this._events[event].splice(this._events[event].indexOf(fct), 1);
  };
  destObj.dispatchEvent   = function(event /* , args... */){
    if(this._events === undefined)  this._events  = {};
    if( this._events[event] === undefined ) return;
    var tmpArray  = this._events[event].slice(); 
    for(var i = 0; i < tmpArray.length; i++){
      var result  = tmpArray[i].apply(this, Array.prototype.slice.call(arguments, 1))
      if( result !== undefined )  return result;
    }
    return undefined
  };
})(VirtualJoystick.prototype);

VirtualJoystick.prototype.deltaX  = function(){ return this._stickX - this._baseX;  }
VirtualJoystick.prototype.deltaY  = function(){ return this._stickY - this._baseY;  }

VirtualJoystick.prototype.up  = function(){
  if( this._pressed === false ) return false;
  var deltaX  = this.deltaX();
  var deltaY  = this.deltaY();
  if( deltaY >= 0 )       return false;
  if( Math.abs(deltaX) > 2*Math.abs(deltaY) ) return false;
  return true;
}
VirtualJoystick.prototype.down  = function(){
  if( this._pressed === false ) return false;
  var deltaX  = this.deltaX();
  var deltaY  = this.deltaY();
  if( deltaY <= 0 )       return false;
  if( Math.abs(deltaX) > 2*Math.abs(deltaY) ) return false;
  return true;  
}
VirtualJoystick.prototype.right = function(){
  if( this._pressed === false ) return false;
  var deltaX  = this.deltaX();
  var deltaY  = this.deltaY();
  if( deltaX <= 0 )       return false;
  if( Math.abs(deltaY) > 2*Math.abs(deltaX) ) return false;
  return true;  
}

VirtualJoystick.prototype.left  = function(){
  if( this._pressed === false ) return false;
  var deltaX  = this.deltaX();
  var deltaY  = this.deltaY();
  if( deltaX >= 0 )       return false;
  if( Math.abs(deltaY) > 2*Math.abs(deltaX) ) return false;
  return true;  
}

VirtualJoystick.prototype._onUp = function()
{
  this._pressed = false; 
  this._stickEl.style.display = "none";

  if(this._stationaryBase == false){  
    this._baseEl.style.display  = "none";

    this._baseX = this._baseY = 0;
    this._stickX  = this._stickY  = 0;
  }
}

VirtualJoystick.prototype._onDown = function(x, y)
{
  this._pressed = true; 
  if(this._stationaryBase == false){
    this._baseX = x;
    this._baseY = y;
    this._baseEl.style.display  = "";
    this._move(this._baseEl.style, (this._baseX - this._baseEl.width /2), (this._baseY - this._baseEl.height/2));
  }

  this._stickX  = x;
  this._stickY  = y;

  if(this._limitStickTravel === true){
    var deltaX  = this.deltaX();
    var deltaY  = this.deltaY();
    var stickDistance = Math.sqrt( (deltaX * deltaX) + (deltaY * deltaY) );
    if(stickDistance > this._stickRadius){
      var stickNormalizedX = deltaX / stickDistance;
      var stickNormalizedY = deltaY / stickDistance;

      this._stickX = stickNormalizedX * this._stickRadius + this._baseX;
      this._stickY = stickNormalizedY * this._stickRadius + this._baseY;
    }   
  }

  this._stickEl.style.display = "";
  this._move(this._stickEl.style, (this._stickX - this._stickEl.width /2), (this._stickY - this._stickEl.height/2));  
}
VirtualJoystick.prototype._onMove = function(x, y)
{
  if( this._pressed === true ){
    this._stickX  = x;
    this._stickY  = y;

    if(this._limitStickTravel === true){
      var deltaX  = this.deltaX();
      var deltaY  = this.deltaY();
      var stickDistance = Math.sqrt( (deltaX * deltaX) + (deltaY * deltaY) );
      if(stickDistance > this._stickRadius){
        var stickNormalizedX = deltaX / stickDistance;
        var stickNormalizedY = deltaY / stickDistance;

        this._stickX = stickNormalizedX * this._stickRadius + this._baseX;
        this._stickY = stickNormalizedY * this._stickRadius + this._baseY;
      }     
    }

          this._move(this._stickEl.style, (this._stickX - this._stickEl.width /2), (this._stickY - this._stickEl.height/2));  
  } 
}

//    bind touch events (and mouse events for debug)

VirtualJoystick.prototype._onMouseUp  = function(event)
{
  return this._onUp();
}

VirtualJoystick.prototype._onMouseDown  = function(event)
{
  event.preventDefault();
  var x = event.clientX;
  var y = event.clientY;
  return this._onDown(x, y);
}

VirtualJoystick.prototype._onMouseMove  = function(event)
{
  var x = event.clientX;
  var y = event.clientY;
  return this._onMove(x, y);
}

VirtualJoystick.prototype._onTouchStart = function(event)
{
  // if there is already a touch inprogress do nothing
  if( this._touchIdx !== null ) return;

  // notify event for validation
  var isValid = this.dispatchEvent('touchStartValidation', event);
  if( isValid === false ) return;

  // dispatch touchStart
  this.dispatchEvent('touchStart', event);

  event.preventDefault();
  // get the first who changed
  var touch = event.changedTouches[0];
  // set the touchIdx of this joystick
  this._touchIdx  = touch.identifier;

  // forward the action
  var x   = touch.pageX;
  var y   = touch.pageY;
  return this._onDown(x, y)
}
VirtualJoystick.prototype._onTouchEnd = function(event)
{
  // if there is no touch in progress, do nothing
  if( this._touchIdx === null ) return;

  // dispatch touchEnd
  this.dispatchEvent('touchEnd', event);

  // try to find our touch event
  var touchList = event.changedTouches;
  for(var i = 0; i < touchList.length && touchList[i].identifier !== this._touchIdx; i++);
  // if touch event isnt found, 
  if( i === touchList.length) return;

  // reset touchIdx - mark it as no-touch-in-progress
  this._touchIdx  = null;

//??????
// no preventDefault to get click event on ios
event.preventDefault();

  return this._onUp()
}

VirtualJoystick.prototype._onTouchMove  = function(event)
{
  // if there is no touch in progress, do nothing
  if( this._touchIdx === null ) return;

  // try to find our touch event
  var touchList = event.changedTouches;
  for(var i = 0; i < touchList.length && touchList[i].identifier !== this._touchIdx; i++ );
  // if touch event with the proper identifier isnt found, do nothing
  if( i === touchList.length) return;
  var touch = touchList[i];

  event.preventDefault();

  var x   = touch.pageX;
  var y   = touch.pageY;
  return this._onMove(x, y)
}
// build default stickEl and baseEl
// build the canvas for joystick base
VirtualJoystick.prototype._buildJoystickBase  = function()
{
  var canvas  = document.createElement( 'canvas' );
  canvas.width  = 126;
  canvas.height = 126;

  var ctx   = canvas.getContext('2d');
  ctx.beginPath(); 
  ctx.strokeStyle = this._strokeStyle; 
  ctx.lineWidth = 6; 
  ctx.arc( canvas.width/2, canvas.width/2, 40, 0, Math.PI*2, true); 
  ctx.stroke(); 

  ctx.beginPath(); 
  ctx.strokeStyle = this._strokeStyle; 
  ctx.lineWidth = 2; 
  ctx.arc( canvas.width/2, canvas.width/2, 60, 0, Math.PI*2, true); 
  ctx.stroke();

  return canvas;
}
// build the canvas for joystick stick

VirtualJoystick.prototype._buildJoystickStick = function()
{
  var canvas  = document.createElement( 'canvas' );
  canvas.width  = 86;
  canvas.height = 86;
  var ctx   = canvas.getContext('2d');
  ctx.beginPath(); 
  ctx.strokeStyle = this._strokeStyle; 
  ctx.lineWidth = 6; 
  ctx.arc( canvas.width/2, canvas.width/2, 40, 0, Math.PI*2, true); 
  ctx.stroke();
  return canvas;
}

//    move using translate3d method with fallback to translate > 'top' and 'left'   
//      modified from https://github.com/component/translate and dependents

VirtualJoystick.prototype._move = function(style, x, y)
{
  if (this._transform) {
    if (this._has3d) {
      style[this._transform] = 'translate3d(' + x + 'px,' + y + 'px, 0)';
    } else {
      style[this._transform] = 'translate(' + x + 'px,' + y + 'px)';
    }
  } else {
    style.left = x + 'px';
    style.top = y + 'px';
  }
}

VirtualJoystick.prototype._getTransformProperty = function() 
{
    var styles = [
      'webkitTransform',
      'MozTransform',
      'msTransform',
      'OTransform',
      'transform'
    ];

    var el = document.createElement('p');
    var style;

    for (var i = 0; i < styles.length; i++) {
      style = styles[i];
      if (null != el.style[style]) {
        return style;
        break;
      }
    }         
}
VirtualJoystick.prototype._check3D = function() 
{        
    var prop = this._getTransformProperty();
    // IE8<= doesn't have `getComputedStyle`
    if (!prop || !window.getComputedStyle) return module.exports = false;

    var map = {
      webkitTransform: '-webkit-transform',
      OTransform: '-o-transform',
      msTransform: '-ms-transform',
      MozTransform: '-moz-transform',
      transform: 'transform'
    };

    // from: https://gist.github.com/lorenzopolidori/3794226
    var el = document.createElement('div');
    el.style[prop] = 'translate3d(1px,1px,1px)';
    document.body.insertBefore(el, null);
    var val = getComputedStyle(el).getPropertyValue(map[prop]);
    document.body.removeChild(el);
    var exports = null != val && val.length && 'none' != val;
    return exports;
}

    </script>

    <script>
      console.log("touchscreen is", VirtualJoystick.touchScreenAvailable() ? "available" : "not available");

      var joystick  = new VirtualJoystick({
        container : document.getElementById('container'),
        mouseSupport  : true,
      });
      joystick.addEventListener('touchStart', function(){
        console.log('down')
      })
      joystick.addEventListener('touchEnd', function(){
        console.log('up')
      })
      var xhttp = new XMLHttpRequest();
      function moveit(dx,dy) 
      {
        xhttp.abort();
        xhttp.open("GET", "/X" + dx + "/Y" + dy, true);
        // xhttp.timeout = 100;
        xhttp.send();
      };
      setInterval(function(){
        var range = 1000.0;
        var dx = (joystick.deltaX()/container.clientWidth)*range;
        var dy = (joystick.deltaY()/container.clientHeight)*range;
        dx = dx.toFixed(2);
        dy = dy.toFixed(2);
        var outputEl  = document.getElementById('result');
        outputEl.innerHTML  = '<b>Result: </b>'
        + 'dx:' + dx
        + ' dy:' + dy
        moveit(dx,dy);
      },  100);

    </script>
  </body>
</html>
 )=====";
